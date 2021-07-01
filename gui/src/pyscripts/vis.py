import sys, os
import pumapy as puma

temp_filepath = "gui/src/pyscripts/gui-ws.vtk"

if len(sys.argv) == 1:  # this is to open a pumapy console from the GUI
	if os.path.exists(temp_filepath):
		ws = puma.import_vti(temp_filepath)
	print('\nImported workspace from GUI into "ws" variable.\n')
	print('import pumapy as puma')

else:
	if os.path.exists(temp_filepath):
		ws = puma.import_vti(temp_filepath)
		cutoff = (int(sys.argv[1]), int(sys.argv[2]))

		if sys.argv[3] == "1":
			print("Creating contour rendering")
			puma.render_contour(ws, cutoff, color=False, background=(1, 1, 1))
		else:
			print("Creating volume rendering")
			puma.render_volume(ws, cutoff, color=False, background=(1, 1, 1))
	else: 
		print("gui-ws.vtk not found. Is it empty?")
