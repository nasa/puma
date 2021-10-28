import unittest
import pumapy as puma
import numpy as np


# the following works locally, but not on github workflow
# import multiprocessing
#
# def test_plot_slices():
#     ws = puma.import_3Dtiff(puma.path_to_example_file("100_fiberform.tif"), 1.3e-6)
#     puma.plot_slices(ws)
#
# def test_compare_slices():
#     ws = puma.import_3Dtiff(puma.path_to_example_file("100_fiberform.tif"), 1.3e-6)
#     ws2 = ws.copy()
#     ws2.binarize_range((100, 255))
#     puma.compare_slices(ws, ws2)
#
# def run_test(self_test, function):
#     p = multiprocessing.Process(target=function)
#     p.start()
#     p.join(3)
#
#     if p.is_alive():
#         print("Function executed for 3 seconds with no errors, this is a planned timeout.")
#         p.terminate()
#         p.join()
#     else:
#         print("Exception raised in detached process.")
#         # self_test.assertEqual(1, 0)
#
# class TestSlicer(unittest.TestCase):
#
#     def test_plot_slices(self):
#         run_test(self, test_plot_slices)
#
#     def test_compare_slices(self):
#         run_test(self, test_compare_slices)


class TestRender(unittest.TestCase):

    def test_render_volume(self):
        ws = puma.Workspace.from_shape_value((5, 5, 5), 1)

        # turn to True to visually inspect plots
        plot = False

        # trying varying different options
        puma.render_volume(ws, cutoff=None, solid_color=(1,1,1), style='surface', origin=(0., 0., 0.),
                           window_size=(1920, 1200), opacity=1., background=(0.3, 0.3, 0.3), show_grid=True, plot_directly=plot,
                           show_axes=True, show_outline=True, cmap='gray', add_to_plot=None, notebook=False)
        puma.render_volume(ws, cutoff=None, solid_color=(1,1,1), style='edges', origin=(0., 0., 0.),
                           window_size=(1920, 1200), opacity=1., background=(0.3, 0.3, 0.3), show_grid=True, plot_directly=plot,
                           show_axes=True, show_outline=True, cmap='gray', add_to_plot=None, notebook=False)
        puma.render_volume(ws, cutoff=None, solid_color=(1,1,1), style='wireframe', origin=(0., 0., 0.),
                           window_size=(1920, 1200), opacity=1., background=(0.3, 0.3, 0.3), show_grid=True, plot_directly=plot,
                           show_axes=True, show_outline=True, cmap='gray', add_to_plot=None, notebook=False)
        puma.render_volume(ws, cutoff=None, solid_color=(1,1,1), style='points', origin=(0., 0., 0.),
                           window_size=(1920, 1200), opacity=1., background=(0.3, 0.3, 0.3), show_grid=True, plot_directly=plot,
                           show_axes=True, show_outline=True, cmap='gray', add_to_plot=None, notebook=False)
        p = puma.render_volume(ws, cutoff=None, solid_color=(1,1,1), style='points', origin=(0., 0., 0.),
                               window_size=(1920, 1200), opacity=1., background=(0.3, 0.3, 0.3), show_grid=True, plot_directly=plot,
                               show_axes=True, show_outline=True, cmap='gray', add_to_plot=None, notebook=False)
        puma.render_volume(ws, cutoff=(1, 1), solid_color=None, style='points', origin=(0., 0., 0.),
                           window_size=(1920, 1200), opacity=1., background=(0.3, 0.3, 0.3), show_grid=False, plot_directly=plot,
                           show_axes=False, show_outline=False, cmap='gray', add_to_plot=p, notebook=False)

    def test_render_contour(self):
        ws = puma.import_3Dtiff(puma.path_to_example_file("100_fiberform.tif"), 1.3e-6)

        # turn to True to visually inspect plots
        plot = False

        # trying varying different options
        puma.render_contour(ws, cutoff=(90, 255), solid_color=(1., 1., 1.), style='surface', origin=(0., 0., 0.),
                            window_size=(1920, 1200), opacity=1., background=(0.3, 0.3, 0.3), show_grid=True, plot_directly=plot,
                            show_axes=True, show_outline=True, add_to_plot=None, notebook=False)
        puma.render_contour(ws, cutoff=(90, 255), solid_color=(1., 1., 1.), style='edges', origin=(0., 0., 0.),
                            window_size=(1920, 1200), opacity=1., background=(0.3, 0.3, 0.3), show_grid=True, plot_directly=plot,
                            show_axes=True, show_outline=True, add_to_plot=None, notebook=False)
        puma.render_contour(ws, cutoff=(90, 255), solid_color=(1., 1., 1.), style='wireframe', origin=(0., 0., 0.),
                            window_size=(1920, 1200), opacity=1., background=(0.3, 0.3, 0.3), show_grid=True, plot_directly=plot,
                            show_axes=True, show_outline=True, add_to_plot=None, notebook=False)
        puma.render_contour(ws, cutoff=(90, 255), solid_color=(1., 1., 1.), style='points', origin=(0., 0., 0.),
                            window_size=(1920, 1200), opacity=1., background=(0.3, 0.3, 0.3), show_grid=True, plot_directly=plot,
                            show_axes=False, show_outline=False, add_to_plot=None, notebook=False)

    def test_render_orientation(self):
        ws = puma.Workspace.from_shape_vector((5, 6, 2), (0.4, 2, 5))

        # turn to True to visually inspect plots
        plot = False

        # trying varying different options
        puma.render_orientation(ws, scale_factor=1., solid_color=None, style='surface', origin=(0., 0., 0.),
                                window_size=(1920, 1200), opacity=1., background=(0.3, 0.3, 0.3), show_grid=True, cmap=None,
                                plot_directly=plot, show_axes=True, show_outline=True, add_to_plot=None, notebook=False,
                                sampling=None)
        puma.render_orientation(ws, scale_factor=0.5, solid_color=None, style='points', origin=(0., 0., 0.),
                                window_size=(1920, 1200), opacity=1., background=(0.3, 0.3, 0.3), show_grid=True, cmap=None,
                                plot_directly=plot, show_axes=True, show_outline=True, add_to_plot=None, notebook=False,
                                sampling=1000)
        puma.render_orientation(ws, scale_factor=0.5, solid_color=None, style='wireframe', origin=(0., 0., 0.),
                                window_size=(1920, 1200), opacity=1., background=(0.3, 0.3, 0.3), show_grid=True, cmap=None,
                                plot_directly=plot, show_axes=True, show_outline=True, add_to_plot=None, notebook=False,
                                sampling=5)
        puma.render_orientation(ws, scale_factor=1., solid_color=None, style='edges', origin=(0., 0., 0.),
                                window_size=(1920, 1200), opacity=1., background=(0.3, 0.3, 0.3), show_grid=True, cmap=None,
                                plot_directly=plot, show_axes=False, show_outline=False, add_to_plot=None, notebook=False,
                                sampling=None)

    def test_render_warp(self):
        ws = puma.Workspace.from_shape_value((5, 6, 2), 1)
        ws.voxel_length = 1
        ws.orientation = np.random.random_sample((5, 6, 2, 3))
        ws.orientation /= ws.orientation_magnitude()[:, :, :, np.newaxis]  # normalize to unit vectors

        # turn to True to visually inspect plots
        plot = False

        # trying varying different options
        puma.render_warp(ws, scale_factor=1., color_by='magnitude', style='surface', origin=(0., 0., 0.),
                         window_size=(1920, 1200), opacity=1., background=(0.3, 0.3, 0.3), show_grid=True, cmap='jet',
                         plot_directly=plot, show_axes=True, show_outline=True, add_to_plot=None, notebook=False)
        puma.render_warp(ws, scale_factor=1., color_by='x', style='points', origin=(0., 0., 0.),
                         window_size=(1920, 1200), opacity=1., background=(0.3, 0.3, 0.3), show_grid=True, cmap='jet',
                         plot_directly=plot, show_axes=False, show_outline=False, add_to_plot=None, notebook=False)
        puma.render_warp(ws, scale_factor=1., color_by='y', style='wireframe', origin=(0., 0., 0.),
                         window_size=(1920, 1200), opacity=1., background=(0.3, 0.3, 0.3), show_grid=True, cmap='jet',
                         plot_directly=plot, show_axes=False, show_outline=False, add_to_plot=None, notebook=False)
        puma.render_warp(ws, scale_factor=1., color_by='z', style='edges', origin=(0., 0., 0.),
                         window_size=(1920, 1200), opacity=1., background=(0.3, 0.3, 0.3), show_grid=True, cmap='jet',
                         plot_directly=plot, show_axes=False, show_outline=False, add_to_plot=None, notebook=False)

    def test_render_contour_multiphase(self):
        ws = puma.import_3Dtiff(puma.path_to_example_file("100_fiberform.tif"), 1.3e-6)

        # turn to True to visually inspect plots
        plot = False

        # trying varying different options
        puma.render_contour_multiphase(ws, cutoffs=((100, 150), (150, 255)), solid_colors=None, style='surface', origin=(0., 0., 0.),
                                       window_size=(1920, 1200), opacity=1., background=(0.3, 0.3, 0.3), show_grid=True,
                                       plot_directly=plot, show_axes=True, show_outline=True, add_to_plot=None, notebook=False)
        puma.render_contour_multiphase(ws, cutoffs=((100, 150), (150, 230), (230, 255)), solid_colors=((1,1,1), (0.6, 0.6, 0.6), (0.3, 0.3, 0.3)),
                                       style='surface', origin=(0., 0., 0.),
                                       window_size=(1920, 1200), opacity=1., background=(0.3, 0.3, 0.3), show_grid=True,
                                       plot_directly=plot, show_axes=False, show_outline=True, add_to_plot=None, notebook=False)
        puma.render_contour_multiphase(ws, cutoffs=((100, 150), (150, 255)), solid_colors=None, style='wireframe', origin=(0., 0., 0.),
                                       window_size=(1920, 1200), opacity=1., background=(0.3, 0.3, 0.3), show_grid=True,
                                       plot_directly=plot, show_axes=True, show_outline=False, add_to_plot=None, notebook=False)
        puma.render_contour_multiphase(ws, cutoffs=((100, 150), (150, 255)), solid_colors=None, style='edges', origin=(0., 0., 0.),
                                       window_size=(1920, 1200), opacity=1., background=(0.3, 0.3, 0.3), show_grid=False,
                                       plot_directly=plot, show_axes=True, show_outline=True, add_to_plot=None, notebook=False)


if __name__ == '__main__':
    unittest.main()
