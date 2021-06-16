from matplotlib import pyplot as plt
import numpy as np
import copy
import sys
import threading
import bisect
from pumapy import print_warning
from scipy import ndimage as ndi
from skimage import segmentation as seg
from skimage import morphology as morph
from pumapy.visualization.slicer import IndexTracker
from pumapy.io.output import export_vti
from mpl_toolkits.axes_grid1 import make_axes_locatable


def label_tows_fm(img_in, mask_in, slice_direction, partial_labels=None, img_ids=None, ids_length=None,
                  template_size=9, search_size_x=11, search_size_y=13,
                  indices=None, slice_frequency=None, export=False, filepathname=''):
    """ Label tows by feature matching.

    This has three steps (the first two are skipped if partial_labels, img_ids and ids_length provided):
    1. Pick tow centroids of specific slices which will serve as anchor points in the tow tracking. These points are
    used as the markers to apply the watershed algorithm on the 2D slices. Lines preventing errors coming from the tow
    contacts can also be drawn.
    2. Track ids using template matching
    3. Match partial labels given from step 1 with ids and apply watershed to get final labels

    :param img_in: domain
    :type img_in: Workspace or ndarray
    :param mask_in: mask
    :type mask_in: Workspace or ndarray
    :param slice_direction: 'x', 'y', 'z'
    :type slice_direction: string
    :param partial_labels: as outputted by TowLabelerFM.pick_centroids()
    :type partial_labels: ndarray, optional
    :param img_ids: as outputted by TowLabelerFM.track_ids()
    :type img_ids: ndarray, optional
    :param ids_length: as outputted by TowLabelerFM.track_ids()
    :type ids_length: ndarray, optional
    :param template_size: with the size of the template kernel
    :type template_size: int, optional
    :param search_size_x: with the x dimension of the search area to compare the template
    :type search_size_x: int, optional
    :param search_size_y: with the y dimension of the search area to compare the template
    :type search_size_y: int, optional
    :param indices: specifying the slices to be analyzed
    :type indices: list(ints), optional
    :param slice_frequency: if indices not provided, how often to label slices
    :type slice_frequency: int, optional
    :param export: specifying whether to export partial_labels, img_ids and ids_length (if computed)
    :type export: bool, optional
    :param filepathname: with path and name of the file to export
    :type filepathname: string, optional
    :return: segmented image and matched ids
    :rtype: tuple(ndarray, ndarray)
    """
    mask, img = TowLabelerFM.error_check(mask_in, img_in, slice_direction, partial_labels, img_ids, ids_length,
                                         template_size, search_size_x, search_size_y)
    if mask is None:
        return None, None

    tow_labeler = TowLabelerFM(img, mask, partial_labels, img_ids, ids_length,
                               indices, slice_direction, slice_frequency, 2*template_size,
                               template_size, search_size_x, search_size_y)

    if img_ids is None or ids_length is None:
        tow_labeler.track_ids()
        if export:
            export_vti(filepathname + "_ids",
                       {"ids": tow_labeler.rotate_for_xy_back(tow_labeler.img_ids, slice_direction),
                        "ids_length": tow_labeler.rotate_for_xy_back(tow_labeler.ids_length, slice_direction)})

    if partial_labels is None:
        tow_labeler.pick_centroids()  # has to be run after because of the slice_frequency based on ids_length mean
        if export:
            export_vti(filepathname + "_partial_labels", tow_labeler.partial_labels)

    tow_labeler.match_ids_labels()
    tow_labeler.img_ids_match = tow_labeler.rotate_for_xy_back(tow_labeler.img_ids_match, slice_direction)
    tow_labeler.img_out = tow_labeler.rotate_for_xy_back(tow_labeler.img_out, slice_direction)
    if export:
        export_vti(filepathname + "_ids_matched", {"ids_matched": tow_labeler.img_ids_match,
                                                   "labels": tow_labeler.img_out})

    return tow_labeler.img_out, tow_labeler.img_ids_match


class TowLabeler:
    def __init__(self, mask_in, indices, slice_direction, slice_frequency, first_index, partial_labels):
        self.slice_direction = slice_direction
        self.mask, self.axis_labels = self.rotate_for_xy(mask_in, slice_direction, return_axis_labels=True)

        if partial_labels is None:
            self.slice_frequency = slice_frequency
            self.first_index = first_index
            self.indices = indices
            self.partial_labels = None
            self.points = []
            self.prev_points = None
            self.padding = 10
            self.lines_mat_removal = []
            self.xcoords_mat_removal = []
            self.ycoords_mat_removal = []
            self.lines_mat_addition = []
            self.xcoords_mat_addition = []
            self.ycoords_mat_addition = []
            self.slice_markers = None
            self.slice_labels = None
            self.prev_slice_labels = None
            self.fig = None
            self.ax = None
            self.cid_centroid = None
            self.cid_lines = None
            self.ims = [None, None]
            self.mask_slice = None
            self.k = None
            self.k_v = None
            self.unique_marker = 1
            self.thread_debounce = None
        else:
            self.partial_labels = self.rotate_for_xy(partial_labels, slice_direction)[0]
            if self.slice_direction == 'x':
                self.indices = np.where(self.partial_labels.sum(axis=(1, 2)) > 0)[0]
            elif self.slice_direction == 'y':
                self.indices = np.where(self.partial_labels.sum(axis=(0, 2)) > 0)[0]
            else:
                self.indices = np.where(self.partial_labels.sum(axis=(0, 1)) > 0)[0]
            self.slice_frequency = self.indices[1] - self.indices[0]

    def pick_centroids(self):

        self.mask = self.rotate_domain_mpl(self.mask)

        total_unique_markers = 0
        if self.indices is None:
            self.indices = [self.first_index]
            while self.indices[-1] + self.slice_frequency < self.mask.shape[2] - self.first_index:
                self.indices.append(self.indices[-1] + self.slice_frequency)

        self.partial_labels = np.zeros_like(self.mask, dtype=int)
        self.slice_markers = np.zeros((self.mask.shape[0], self.mask.shape[1]), dtype=int)
        for self.k, self.k_v in enumerate(self.indices):
            while True:
                # picking centroids for k_v slice
                self.reset_data()
                self._plot1_pick_points()
                self._plot2_pick_lines()
                self._plot3_watershed_labels()

                # if slice segmentation accepted, save data
                if input("Accept watershed on Slice " + str(self.k_v) + "? Enter 1 to accept 0 to reject.") == "1":
                    self.slice_markers[self.slice_markers > 0] += total_unique_markers
                    self.slice_labels[self.slice_labels > 0] += total_unique_markers
                    self.partial_labels[:, :, self.k_v] = self.slice_labels.copy()
                    total_unique_markers += self.unique_marker - 1
                    self.prev_points = copy.deepcopy(self.points)
                    self.prev_slice_labels = self.slice_labels.copy()
                    break

        self.partial_labels = self.rotate_domain_mpl_back(self.partial_labels)
        self.mask = self.rotate_domain_mpl_back(self.mask)

    def reset_data(self):
        self.mask_slice = self.mask[:, :, self.k_v].copy()
        self.points.clear()
        self.lines_mat_removal.clear()
        self.xcoords_mat_removal.clear()
        self.ycoords_mat_removal.clear()
        self.lines_mat_addition.clear()
        self.xcoords_mat_addition.clear()
        self.ycoords_mat_addition.clear()
        self.slice_markers.fill(0)
        self.unique_marker = 1

    def _plot_slice(self, img, prev_slice_labels, title_text, colormap='gray'):
        self.fig, self.ax = plt.subplots(1, 2, figsize=(14, 7))
        if prev_slice_labels is not None:
            self.ims[0] = self.ax[0].imshow(prev_slice_labels, cmap='jet', vmin=prev_slice_labels.min(),
                                            vmax=prev_slice_labels.max())
            [self.ax[0].plot(point[0][1], point[0][0], 'o') for point in self.prev_points]
            self.ax[0].set_xlabel(self.axis_labels[0])
            self.ax[0].set_ylabel(self.axis_labels[1])
            plt.colorbar(self.ims[0], cax=make_axes_locatable(self.ax[0]).append_axes("right", size="5%", pad=0.05),
                         ax=self.ax, ticks=np.linspace(img.min(), img.max(), 15, endpoint=True).astype(int))
            self.ax[0].set_title("Previous slice labels. Need to pick {} points.".format(len(self.prev_points)))
            self.ax[0].grid(linestyle=':')

        self.ims[1] = self.ax[1].imshow(img, cmap=colormap, vmin=img.min(), vmax=img.max())
        self.ax[1].set_xlabel(self.axis_labels[0])
        self.ax[1].set_ylabel(self.axis_labels[1])
        plt.colorbar(self.ims[1], cax=make_axes_locatable(self.ax[1]).append_axes("right", size="5%", pad=0.05),
                     ax=self.ax, ticks=np.linspace(img.min(), img.max(), 15, endpoint=True).astype(int))
        self.ax[1].set_title(title_text)
        self.ax[1].grid(linestyle=':')

    def _plot1_pick_points(self):
        while True:
            self._plot_slice(self.mask_slice, self.prev_slice_labels,
                             'Pick points for slice: {}. Slices to analyze: {}/{}'
                             .format(self.k_v, self.k + 1, len(self.indices)))
            self.cid_centroid = self.fig.canvas.mpl_connect('button_press_event', self._onclick_points)
            plt.show(block=True)
            self.fig.canvas.mpl_disconnect(self.cid_centroid)

            if self.prev_points is not None and len(self.prev_points) != len(self.points):
                print_warning("Number of centroids selected doesn't match previous slice. Pick again.")
                self.reset_data()
            else:
                break

    def _onclick_points(self, event):
        if event.inaxes == self.ax[1]:
            if event.xdata is not None and event.ydata is not None:
                if 1 <= event.xdata < self.mask.shape[1] and 1 <= event.ydata < self.mask.shape[0]:
                    index = (int(np.round(event.ydata)), int(np.round(event.xdata)))
                    if self.mask[index[0], index[1], self.k_v]:
                        self.ax[1].plot(index[1], index[0], 'o')
                        self.ax[1].annotate(f'{(index[1], index[0])}',
                                            xy=(index[1], index[0]),
                                            xytext=(15, 15), textcoords="offset points",
                                            bbox=dict(boxstyle="round", fc="w", alpha=0.75),
                                            arrowprops=dict(arrowstyle="->", fc="b", alpha=0.75))
                        self.points.append([index, self.unique_marker])
                        self.ax[1].set_title('Pick points for slice: {}. Slices to analyze: {}/{}. Points picked: {}'
                                             .format(self.k_v, self.k + 1, len(self.indices), len(self.points)))
                        self.unique_marker += 1
                        self.fig.canvas.draw()

    def _plot2_pick_lines(self):
        mask_slice_padded = np.zeros((self.mask_slice.shape[0] + 2*self.padding,
                                      self.mask_slice.shape[1] + 2*self.padding))
        mask_slice_padded[self.padding:-self.padding, self.padding:-self.padding] = self.mask_slice
        self._plot_slice(mask_slice_padded, self.prev_slice_labels,
                         'Draw watershed lines for slice: {}. Slices to analyze: {}/{}'
                         .format(self.k_v, self.k + 1, len(self.indices)))
        [self.ax[1].plot(point[0][1] + self.padding, point[0][0] + self.padding, 'o') for point in self.points]
        self.cid_lines = self.fig.canvas.mpl_connect('button_press_event', self._onpress_lines)
        plt.show(block=True)
        self.fig.canvas.mpl_disconnect(self.cid_lines)

        mask_mat_removal = self._modify_mask_with_lines(self.lines_mat_removal)
        mask_slice_padded[mask_mat_removal] = 0
        mask_slice_padded = morph.binary_erosion(mask_slice_padded)

        mask_mat_addition = self._modify_mask_with_lines(self.lines_mat_addition)
        mask_mat_addition = morph.binary_dilation(mask_mat_addition)
        mask_slice_padded[mask_mat_addition] = 1

        self.mask_slice = mask_slice_padded[self.padding:-self.padding, self.padding:-self.padding]

    def _modify_mask_with_lines(self, lines):
        discretized_lines = []
        for line in lines:
            discretized_lines.append((np.linspace(line[0][0], line[0][1], 100),
                                      np.linspace(line[1][0], line[1][1], 100)))
        discretized_lines = np.round(np.array(discretized_lines)).astype(int)

        mask_modify = np.zeros((self.mask_slice.shape[0] + 2*self.padding,
                                self.mask_slice.shape[1] + 2*self.padding), dtype=bool)
        for line in range(discretized_lines.shape[0]):
            mask_modify[discretized_lines[line, 1], discretized_lines[line, 0]] = 1
        return mask_modify

    def _onpress_lines(self, event):
        if self.thread_debounce is None:
            self.thread_debounce = threading.Timer(0.25, self._onsingleclick_lines, [event])
            self.thread_debounce.start()
        if event.dblclick:
            self.thread_debounce.cancel()
            self._ondoubleclick_lines(event)

    def _onsingleclick_lines(self, event):
        if event.inaxes == self.ax[1]:
            if event.xdata is not None and event.ydata is not None:
                if (1 <= event.xdata < self.mask.shape[1] + 2 * self.padding and
                        1 <= event.ydata < self.mask.shape[0] + 2 * self.padding):
                    self.xcoords_mat_removal.append(event.xdata)
                    self.ycoords_mat_removal.append(event.ydata)
                    self.ax[1].plot(event.xdata, event.ydata, 'ro')
                    if len(self.xcoords_mat_removal) == 2:
                        self.ax[1].plot(self.xcoords_mat_removal, self.ycoords_mat_removal, 'r.--')
                        self.lines_mat_removal.append((copy.deepcopy(self.xcoords_mat_removal),
                                                       copy.deepcopy(self.ycoords_mat_removal)))
                        self.xcoords_mat_removal.clear()
                        self.ycoords_mat_removal.clear()
                    self.fig.canvas.draw()
        self.thread_debounce = None

    def _ondoubleclick_lines(self, event):
        if event.inaxes == self.ax[1]:
            if event.xdata is not None and event.ydata is not None:
                if (1 <= event.xdata < self.mask.shape[1] + 2 * self.padding and
                        1 <= event.ydata < self.mask.shape[0] + 2 * self.padding):
                    self.xcoords_mat_addition.append(event.xdata)
                    self.ycoords_mat_addition.append(event.ydata)
                    self.ax[1].plot(event.xdata, event.ydata, 'bo')
                    if len(self.xcoords_mat_addition) == 2:
                        self.ax[1].plot(self.xcoords_mat_addition, self.ycoords_mat_addition, 'b.--')
                        self.lines_mat_addition.append((copy.deepcopy(self.xcoords_mat_addition),
                                                        copy.deepcopy(self.ycoords_mat_addition)))
                        self.xcoords_mat_addition.clear()
                        self.ycoords_mat_addition.clear()
                    self.fig.canvas.draw()
        self.thread_debounce = None

    def _plot3_watershed_labels(self):
        for point in self.points:
            self.slice_markers[point[0]] = point[1]

        # compute distance transform for labels expansion
        dist_transform = ndi.distance_transform_edt(self.mask_slice)
        self.slice_labels = seg.watershed(-dist_transform, self.slice_markers, mask=self.mask_slice)

        self._plot_slice(self.slice_labels, self.prev_slice_labels,
                         'Watershed labels for slice: {}. Slices to analyze: {}/{}'
                         .format(self.k_v, self.k + 1, len(self.indices)), colormap='jet')
        [self.ax[1].plot(point[0][1], point[0][0], 'o') for point in self.points]
        plt.show(block=True)

    @staticmethod
    def rotate_for_xy(img, slice_direction, return_axis_labels=False):
        if slice_direction == 'x':
            img = np.swapaxes(img, 0, 2)
            img = np.swapaxes(img, 0, 1)
        elif slice_direction == 'y':
            img = np.swapaxes(img, 1, 2)
            img = np.swapaxes(img, 1, 0)
        if return_axis_labels:
            if slice_direction == 'x':
                axis_labels = ('y', 'z')
            elif slice_direction == 'y':
                axis_labels = ('z', 'x')
            else:
                axis_labels = ('x', 'y')
            return img, axis_labels
        else:
            return img

    @staticmethod
    def rotate_for_xy_back(img, slice_direction):
        if slice_direction == 'x':
            img = np.swapaxes(img, 1, 0)
            img = np.swapaxes(img, 2, 0)
        elif slice_direction == 'y':
            img = np.swapaxes(img, 0, 1)
            img = np.swapaxes(img, 2, 1)
        return img

    @staticmethod
    def rotate_domain_mpl(img):
        img = np.rot90(img, axes=(1, 0))
        img = np.flip(img, axis=1)
        return img

    @staticmethod
    def rotate_domain_mpl_back(img):
        img = np.flip(img, axis=1)
        img = np.rot90(img, axes=(0, 1))
        return img


class TowLabelerFM(TowLabeler):
    def __init__(self, img_in, mask_in, partial_labels, img_ids, ids_length,
                 indices, slice_direction, slice_frequency, first_index,
                 t_size, search_size_x, search_size_y):

        self.img = self.rotate_for_xy(img_in, slice_direction)
        super().__init__(mask_in, indices, slice_direction, slice_frequency, first_index, partial_labels)

        # ids tracking output
        if img_ids is None or ids_length is None:
            self.img_ids = np.zeros_like(self.img, dtype=np.uint32)

            # kernel info
            self.t_size = t_size
            self.search_size_x = search_size_x
            self.search_size_y = search_size_y
            self.mid_t = t_size // 2
            self.mid_s = self.search_size_x // 2, self.search_size_y // 2
            self.nt_search = self.search_size_x - self.t_size + 1, self.search_size_y - self.t_size + 1
            self.mid_nt_search = self.nt_search[0] // 2, self.nt_search[1] // 2
            self.valid_voxs = None
            self.k_slice_t = None
            self.kp_slice_s = None

            # restricting the domain to kernels
            self.partial_labels_match = None
            self.mask_int = self.mask.copy()
            self.mask_int[np.r_[:self.mid_s[0], self.mask.shape[0] - self.mid_s[0]:self.mask.shape[0]]] = 0
            self.mask_int[:, np.r_[:self.mid_s[1], self.mask.shape[1] - self.mid_s[1]:self.mask.shape[1]]] = 0
        else:
            self.img_ids = self.rotate_for_xy(img_ids, slice_direction)[0]
            self.ids_length = self.rotate_for_xy(ids_length, slice_direction)[0]
            if partial_labels is None:  # if we have to pick centroids
                self.slice_frequency = int(np.round(np.mean(self.ids_length[self.ids_length > 1])))
                print("Using slice_frequency = {} based on ids_length mean.".format(self.slice_frequency))

        # label matching
        self.label_dict = dict()
        self.img_ids_match = np.zeros_like(self.img_ids, dtype=int)
        self.mask_good_ids = np.zeros_like(self.img_ids, dtype=bool)
        self.img_out = np.zeros_like(self.img_ids, dtype=int)

    def track_ids(self):
        self._assign_unique_ids()
        self._create_templates()
        for k in range(self.img.shape[2] - 1):
            self._extract_template_and_search_areas(k)
            self._find_min_vox(k)
            sys.stdout.write("\rPerforming convolutions ... {:.1f}% ".format(k / (self.img.shape[2] - 2) * 100))
        print("Done")
        self.img_ids[~self.mask] = 0
        self._find_ids_length()
        self.slice_frequency = int(np.round(np.mean(self.ids_length[self.ids_length > 1])))
        print("Using slice_frequency = {} based on ids_length mean.".format(self.slice_frequency))

    def match_ids_labels(self):
        self.partial_labels_match = self.partial_labels.copy()
        self._create_labels_ids_dict()

        for ind in range(1, len(self.indices)):
            self._label_voting(ind, self._find_slice_unique_labels(ind - 1), self._find_slice_unique_labels(ind))
            print("\nLabel matching ... {:.1f}% ".format(ind / (len(self.indices) - 1) * 100))

        self._watershed_labels()

    def _assign_unique_ids(self):
        in_img = self.img[self.mid_s[0]:-self.mid_s[0], self.mid_s[1]:-self.mid_s[1], self.mid_t:-self.mid_t]
        self.img_ids[self.mid_s[0]:-self.mid_s[0],
        self.mid_s[1]:-self.mid_s[1],
        self.mid_t:-self.mid_t] = np.random.choice(in_img.size, in_img.size,
                                                   replace=False).reshape(in_img.shape) + 1

    def _create_templates(self):
        print("Creating templates ... ", end='')
        self.templates = np.squeeze(np.lib.stride_tricks.as_strided(self.img,
                                                                    (self.img.shape[0] - self.t_size + 1,
                                                                     self.img.shape[1] - self.t_size + 1,
                                                                     self.img.shape[2],
                                                                     self.t_size, self.t_size, 1),
                                                                    2 * self.img.strides))
        print("Done")

    def _extract_template_and_search_areas(self, k):
        # extract templates at k slice
        self.k_slice_t = self.templates[self.mask_int[self.mid_t:-self.mid_t, self.mid_t:-self.mid_t, k], k]

        # extract search areas at k-1 slice
        self.valid_voxs = np.count_nonzero(self.mask_int[:, :, k])
        self.kp_slice_s = np.zeros((self.valid_voxs, self.nt_search[0], self.nt_search[1], self.t_size, self.t_size))
        valid_counter = 0
        for i in range(self.mid_s[0], self.img.shape[0] - self.mid_s[0]):
            for j in range(self.mid_s[1], self.img.shape[1] - self.mid_s[1]):
                if self.mask_int[i, j, k]:
                    self.kp_slice_s[valid_counter] = self.templates[i - self.mid_t - self.mid_nt_search[0]:
                                                                    i - self.mid_t + self.mid_nt_search[0] + 1,
                                                     j - self.mid_t - self.mid_nt_search[1]:
                                                     j - self.mid_t + self.mid_nt_search[1] + 1, k + 1]
                    valid_counter += 1

    def _find_min_vox(self, k):
        # sum of squared difference as A^2 + B^2 - 2*AB
        ssd = np.einsum('opijk, ojk -> opi', self.kp_slice_s, self.k_slice_t)
        ssd *= - 2
        ssd += np.einsum('opijk, opijk -> opi', self.kp_slice_s, self.kp_slice_s)
        ssd += np.einsum('ojk, ojk -> o', self.k_slice_t, self.k_slice_t)[:, np.newaxis, np.newaxis]

        # find most similar patch in k-1 search area as compared to k template
        min_voxs = np.unravel_index(ssd.reshape(ssd.shape[0], -1).argmin(-1), ssd.shape[-2:])

        # filling new layer of ids
        i_ind, j_ind = np.where(self.mask_int[:, :, k])
        self.img_ids[i_ind - self.mid_s[0] + self.mid_t + min_voxs[0],
                     j_ind - self.mid_s[1] + self.mid_t + min_voxs[1], k + 1] = self.img_ids[self.mask_int[:, :, k], k]

    def _find_ids_length(self):
        ids = np.unique(self.img_ids)
        ids_distances = np.zeros((2, ids.shape[0]), dtype=int)
        ids_distances[0] = ids
        for k in range(self.mid_t + 1, self.img_ids.shape[2] - self.mid_t):
            ids_k = np.unique(self.img_ids[:, :, k])
            if ids_k[0] == 0:
                ids_k = ids_k[1:]
            ids_k_indices = np.searchsorted(ids, ids_k)
            ids_distances[1, ids_k_indices] += 1
        keyarray_distances = np.zeros(np.max(ids) + 1, dtype=ids.dtype)
        keyarray_distances[ids_distances[0]] = ids_distances[1]
        self.ids_length = keyarray_distances[self.img_ids]

    def _create_labels_ids_dict(self):

        self.img_ids[self.ids_length < self.slice_frequency] = 0

        # match slices partial labels
        self.unique_labels = np.unique(self.partial_labels[self.partial_labels != 0])
        print("Create labels-ids dictionary ... ", end='')
        for label in self.unique_labels:
            label_mask = self.partial_labels == label
            unique_ids = np.unique(self.img_ids[label_mask])
            unique_ids = unique_ids[unique_ids != 0]
            self.label_dict[label] = unique_ids
        print("Done")

    def _find_slice_unique_labels(self, ind):
        ind_labels = np.unique(self.partial_labels[:, :, self.indices[ind]])
        ind_labels = ind_labels[ind_labels != 0]
        return ind_labels

    def _label_voting(self, ind, km_labels, k_labels):
        # check label at slice k
        for label_counter, k_label in enumerate(k_labels):
            km_label_votes = []
            intersection = dict()

            # see which label at slice k-1 has the most ids in common
            for km_label in km_labels:
                intersection[km_label] = np.intersect1d(self.label_dict[k_label], self.label_dict[km_label],
                                                        assume_unique=False)
                bisect.insort(km_label_votes, (len(intersection[km_label]), km_label))

            tracked_label = self.partial_labels_match[self.partial_labels == km_label_votes[-1][1]][0]

            # assign most popular label at k-1 to k
            self.partial_labels_match[self.partial_labels_match == k_label] = tracked_label

            # color the ids in common like the label
            self.mask_good_ids.fill(False)
            self.mask_good_ids[:, :, self.indices[ind - 1]:
                                     self.indices[ind] + 1] = np.isin(self.img_ids[:, :, self.indices[ind - 1]:
                                                                                         self.indices[ind] + 1],
                                                                      intersection[km_label_votes[-1][1]])
            self.img_ids_match[self.mask_good_ids] = tracked_label
            sys.stdout.write("\rLabel {}/{}".format(label_counter + 1, len(k_labels)))

    def _watershed_labels(self):
        self.img_ids_match[self.partial_labels_match > 0] = self.partial_labels_match[self.partial_labels_match > 0]
        dist_transform = ndi.distance_transform_edt(self.img_ids_match)
        self.img_out = seg.watershed(-dist_transform, self.img_ids_match, mask=self.mask)

    @staticmethod
    def error_check(mask, img, slice_direction, partial_labels, img_ids, ids_length,
                    t_size, search_size_x, search_size_y):
        mask, img = IndexTracker.error_checks(mask.astype(bool), img, slice_direction)

        if partial_labels is not None:
            if TowLabeler.rotate_for_xy(partial_labels, slice_direction)[0].shape != \
                    TowLabeler.rotate_for_xy(mask, slice_direction).shape:
                raise Exception("Provided partial_labels has to be of the same size as the img and mask.")
        if img_ids is not None:
            if TowLabeler.rotate_for_xy(img_ids, slice_direction)[0].shape != \
                    TowLabeler.rotate_for_xy(mask, slice_direction).shape:
                raise Exception("Provided img_ids has to be of the same size as the img and mask.")
        if ids_length is not None:
            if TowLabeler.rotate_for_xy(ids_length, slice_direction)[0].shape != \
                    TowLabeler.rotate_for_xy(mask, slice_direction).shape:
                raise Exception("Provided ids_length has to be of the same size as the img and mask.")
        if t_size > search_size_x or t_size > search_size_y:
            raise Exception("Search area cannot be smaller than template filter size")
        if t_size % 2 != 1 or search_size_x % 2 != 1 or search_size_y % 2 != 1:
            raise Exception("Only odd dimensions on template filter and search area supported")
        return mask, img
