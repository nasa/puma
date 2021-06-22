import os

path = os.path.join(os.environ["PuMA_DIR"], "python", "tutorials")

files = [f for f in os.listdir(path) if os.path.isfile(os.path.join(path, f))]

for file in files:
	if file[-5:] == "ipynb":
		f = open(file[:-5] + "nblink", "a")
		f.write("{\n    \"path\": \"../../../python/tutorials/"+file+"\"\n}")
		f.close()

print("Updated list of tutorials.")
