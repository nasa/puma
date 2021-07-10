import pysvn, os.path

def ConvertLineEndings(fileName):
    print("Converting line endings for file:", fileName)
    file = open(fileName)
    lines = file.readlines()
    file.close()
    # Convert to '\n' line feeds
    for i in range(len(lines)):
        lines[i] = lines[i].replace('\r\n', '\n')
        lines[i] = lines[i].replace('\r', '\n')
    file = open(fileName, 'w')
    file.writelines(lines)
    file.close()

# Set the Subversion 'svn:eol-style' property for all text files to 'native'
# This will make sure there are no problems with developing code between
# linux and windows systems
textFileExtensions = ['.txt', '.py', '.cpp', '.c', '.h', '.hpp', '.cxx', '.hxx', '.in',
                      '.dot', '.ini', '.nsi', '.tg3', '.i', '.cfl', '.pth']
client = pysvn.Client()
changes = client.status('.')
for f in changes:
    if f.is_versioned:
        root, ext = os.path.splitext(f.path)
        if ext in textFileExtensions:
            prop_list = client.propget('svn:eol-style', f.path)
            key = f.path.replace('\\', '/')
            if key in prop_list and prop_list[key] == 'native':
                pass    # Property already set
            else:
                print("Setting 'svn:eol-style' property to 'native' for:", f.path)
                try:
                    client.propset('svn:eol-style', 'native', f.path)
                except pysvn.ClientError as e:
                    if 'inconsistent newlines' in str(e):
                        ConvertLineEndings(f.path)
                        client.propset('svn:eol-style', 'native', f.path)
                    else:
                        raise
        else:
            print('No end of line conversion for:', f.path)
