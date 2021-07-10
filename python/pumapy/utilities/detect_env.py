def detect_env():

    print("\nDetected pumapy environment:")

    complete_env = True

    try:
        import TexGen.Core
    except ImportError:
        complete_env = False
        print("- TexGen not found, cannot use pumapy.export_weave_vtu()")

    try:
        import dolfin
    except ImportError:
        complete_env = False
        print("- FEniCS/dolfin not found, cannot use pumapy.compute_permeability()")

    try:
        from pumapy.generation.weave_3mdcp.weave_3mdcp import generate_3mdcp
    except ImportError:  # import it only if installed
        print("- generate_3mdcp() not imported.")

    # visualization
    try:
        import paraview.simple
    except ImportError:
        complete_env = False
        print("- Paraview not found, cannot use pumapy.render... and import_vti() functions.")

    # add wrapped puma cpp functions
    # try:
    #     import pumapy.utilities.libPuMA
    # except ImportError:
    # complete_env = False
    #     print("libPuMA not found, cannot use pumapy.cpp functions.")

    if complete_env:
        print("pumapy is correctly installed with ALL the functionalities.")
    else:
        print("pumapy is correctly installed with PARTIAL functionalities.")


if __name__ == "__main__":
    detect_env()
