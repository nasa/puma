from matplotlib import pyplot as plt
import numpy as np
import pumapy.utilities.workspace as ws


def plot_slices(ws_nparray, slice_direction='z', crange=None, cmap='gray', index=1):
    """ Plot slices of domain along a specified direction (z default)

        :param ws_nparray: domain
        :type ws_nparray: Workspace or np.ndarray
        :param slice_direction: 'x', 'y', 'z'
        :type slice_direction: string
        :param crange: color range, i.e. specify min and max grayscale
        :type crange: (int, int)
        :param cmap: color map for the plot, 'gray' (default), 'jet' or refer to matplotlib for other colormaps
        :type cmap: string
        :param index: specifying the slice index in which the plot will be opened
        :type: int
        :return: slicer object
        :rtype: PlotSlicer

        >>> import pumapy as puma
        >>> ws = puma.import_3Dtiff(puma.path_to_example_file("100_fiberform.tif"), 1.3e-6)
        >>> puma.plot_slices(ws)
    """

    img, _ = PlotSlicer.error_checks(ws_nparray, None, slice_direction)
    if img is None:
        return

    img, axis_labels, slices, rows, cols = PlotSlicer.rotate_domain_mpl(img, slice_direction)
    return PlotSlicer(img, slice_direction, crange, cmap, index, axis_labels, slices, rows, cols)


def compare_slices(ws_nparray1, ws_nparray2, slice_direction='z', crange1=None, cmap1='gray',
                   crange2=None, cmap2='gray', index=1):
    """ Plot slices of domain along a specified direction (z default)

        :param ws_nparray1: domain
        :type ws_nparray1: Workspace or np.ndarray
        :param ws_nparray2: domain
        :type ws_nparray2: Workspace or np.ndarray
        :param slice_direction: 'x', 'y', 'z'
        :type slice_direction: string
        :param crange1: color range for plot 1, specify min and max grayscale
        :type crange1: (int, int)
        :param cmap1: color map for plot 1, 'gray' (default), 'jet' or refer to matplotlib for other colormaps
        :type cmap1: string
        :param crange2: color range for plot 2, specify min and max grayscale
        :type crange2: (int, int)
        :param cmap2: color map for plot 2, 'gray' (default), 'jet' or refer to matplotlib for other colormaps
        :type cmap2: string
        :param index: specifying the slice index in which the plot will be opened
        :type index: int
        :return: slicer object
        :rtype: CompareSlicer

        >>> import pumapy as puma
        >>> ws = puma.import_3Dtiff(puma.path_to_example_file("100_fiberform.tif"), 1.3e-6)
        >>> ws2 = ws.copy()
        >>> ws2.binarize_range((100, 255))
        >>> puma.compare_slices(ws, ws2)
    """

    img1, img2 = CompareSlicer.error_checks(ws_nparray1, ws_nparray2, slice_direction)
    if img1 is None:
        return

    img1, axis_labels, slices1, rows1, cols1 = CompareSlicer.rotate_domain_mpl(img1, slice_direction)
    img2, _, slices2, rows2, cols2 = CompareSlicer.rotate_domain_mpl(img2, slice_direction)
    return CompareSlicer(img1, img2, slice_direction, crange1, cmap1, crange2, cmap2, index,
                         axis_labels, slices1, slices2, rows1, cols1, rows2, cols2)


class IndexTracker:
    def __init__(self, img, img2, slice_direction, index, axis_labels, slices, slices_titles):
        self.slice_direction = slice_direction
        self.ind = index - 1
        self.axis_labels = axis_labels
        self.slices = slices
        self.slices_titles = slices_titles
        self.axis_labels = axis_labels
        self.img = img
        self.img2 = img2

    def onkey(self, event):
        if event.key == "right":
            if self.ind < self.slices - 10:
                self.ind += 10
        elif event.key == "left":
            if self.ind >= 10:
                self.ind -= 10
        elif event.key == "up":
            if self.ind < self.slices - 100:
                self.ind += 100
        elif event.key == "down":
            if self.ind >= 100:
                self.ind -= 100
        self.update()

    def onscroll(self, event):
        if event.button == 'down':
            if self.ind != self.slices - 1:
                self.ind += 1
        else:
            if self.ind != 0:
                self.ind -= 1
        self.update()

    def update(self):
        pass  # virtual

    @staticmethod
    def format_coord(x, y):
        return 'x=%1.f, y=%1.f' % (x, y)

    @staticmethod
    def rotate_domain_mpl(img, slice_direction):
        if slice_direction == 'x':
            img = np.swapaxes(img, 0, 2)
            img = np.swapaxes(img, 0, 1)
            slices, rows, cols = img.shape
            axis_labels = ('y', 'z')
        elif slice_direction == 'y':
            img = np.swapaxes(img, 1, 2)
            img = np.swapaxes(img, 1, 0)
            rows, slices, cols = img.shape
            axis_labels = ('z', 'x')
        else:
            rows, cols, slices = img.shape
            axis_labels = ('x', 'y')

        img = np.rot90(img, axes=(1, 0))
        img = np.flip(img, axis=1)
        return img, axis_labels, slices, rows, cols

    @staticmethod
    def _ws_check(img):
        if isinstance(img, ws.Workspace):
            img = img.matrix.copy()
        elif isinstance(img, np.ndarray):
            if img.ndim > 3 or img.ndim < 2:
                raise Exception("Numpy array has to have 3 dimensions")
            elif img.ndim == 2:
                img = img[:, :, np.newaxis]
            img = img.copy()
        else:
            raise Exception("Data to be sliced has to be either a pumapy.Workspace or np.ndarray")
        return img

    @staticmethod
    def error_checks(img, img2, slice_direction):
        slice_direction = slice_direction.lower()
        if not (slice_direction == 'x' or slice_direction == 'y' or slice_direction == 'z'):
            raise Exception("Slice direction can only be along 'x', 'y' or 'z'")

        img = PlotSlicer._ws_check(img)
        if img is None:
            return None, None

        if img2 is not None:
            img2 = PlotSlicer._ws_check(img2)
            if img2 is None:
                return None, None
            if not (slice_direction == 'x' or slice_direction == 'y' or slice_direction == 'z'):
                raise Exception("Slice direction can only be along 'x', 'y' or 'z'")
        return img, img2


class PlotSlicer(IndexTracker):
    def __init__(self, img, slice_direction, color_range, color_map, index, axis_labels, slices, rows, cols):
        super().__init__(img, None, slice_direction, index, axis_labels, slices, None)
        self.color_range = color_range
        self.color_map = color_map
        self.rows = rows
        self.cols = cols

        if self.color_range is None:
            self.color_range = self.img.min(), self.img.max()

        self.fig, self.ax = plt.subplots(1, 1)
        self.im = self.ax.imshow(self.img[:, :, self.ind], cmap=self.color_map,
                                 vmin=self.color_range[0], vmax=self.color_range[1])

        self.cid_scroll = self.fig.canvas.mpl_connect('scroll_event', self.onscroll)
        self.cid_key = self.fig.canvas.mpl_connect('key_press_event', self.onkey)

        self.ax.set_xlabel(self.axis_labels[0])
        self.ax.set_ylabel(self.axis_labels[1])
        plt.colorbar(mappable=self.im, ax=self.ax, fraction=0.046, pad=0.04,
                     ticks=np.linspace(self.color_range[0], self.color_range[1], 15).astype(int))
        self.ax.grid(linestyle=':')
        self.update()
        plt.show(block=True)

    def update(self):
        self.im.set_data(self.img[:, :, self.ind])
        self.ax.set_title('Slice: {}/{} along {}'.format(self.ind + 1, self.slices, self.slice_direction))
        self.ax.format_coord = PlotSlicer.format_coord
        self.fig.canvas.draw()


class CompareSlicer(IndexTracker):
    def __init__(self, img1, img2, slice_direction, color_range1, color_map1, color_range2, color_map2, index,
                 axis_labels, slices1, slices2, rows1, cols1, rows2, cols2):
        super().__init__(img1, img2, slice_direction, index, axis_labels, max(slices1, slices2), [slices1, slices2])
        self.imgs = img1, img2
        self.color_ranges = [color_range1, color_range2]
        self.color_maps = color_map1, color_map2
        self.rows = rows1, rows2
        self.cols = cols1, cols2

        self.fig, self.ax = plt.subplots(1, 2)
        self.fig.tight_layout(pad=3.0)
        self.ims = [None, None]

        self.cid_scroll = self.fig.canvas.mpl_connect('scroll_event', self.onscroll)
        self.cid_key = self.fig.canvas.mpl_connect('key_press_event', self.onkey)

        for i in range(2):
            if self.color_ranges[i] is None:
                self.color_ranges[i] = (self.imgs[i].min(), self.imgs[i].max())

            self.ims[i] = self.ax[i].imshow(self.imgs[i][:, :, self.ind], cmap=self.color_maps[i],
                                            vmin=self.color_ranges[i][0], vmax=self.color_ranges[i][1])
            self.ax[i].set_xlabel(self.axis_labels[0])
            self.ax[i].set_ylabel(self.axis_labels[1])
            plt.colorbar(self.ims[i], ax=self.ax[i], fraction=0.046, pad=0.04,
                         ticks=np.linspace(self.color_ranges[i][0], self.color_ranges[i][1], 15).astype(int))
            self.ax[i].grid(linestyle=':')
        self.update()
        plt.show(block=True)

    def update(self):
        for i in range(2):
            self.ims[i].set_data(self.imgs[i][:, :, self.ind])
            self.ax[i].set_title('Slice: {}/{} along {}'.format(self.ind + 1, self.slices_titles[i], self.slice_direction))
            self.ax[i].format_coord = CompareSlicer.format_coord
        self.fig.canvas.draw()
