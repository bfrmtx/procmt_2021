# python is in  C:\msys64\ucrt64\bin\python.exe
import os
print("python is in ", os.path.dirname(os.path.abspath(__file__)))
import sys
import subprocess
import shutil 
# we need ldd.exe from msys2
sys.path.append(r"C:\msys64\usr\bin")
ldd_exe = r"C:\msys64\usr\bin\ldd.exe"
home_dir = os.path.expanduser("~")
scandir = os.path.join(home_dir, 'mtxsw', 'install', 'procmt', 'bin')
msys2_urcrt_root = r'C:\msys64'
msys2_urcrt_bin = r'C:\msys64\ucrt64\bin' 
windeployqt6 = r'C:\msys64\mingw64\bin\windeployqt6.exe'
# make complete path without backslashes
all_dpendencies = []
# for all executable files in the directory call ldd.exe and fetch the output
def get_dependencies_all(scandir,):
    for root, dirs, files in os.walk(scandir):
        for file in files:
            if file.endswith('.exe'):
                executable = os.path.join(root, file)
                print(f"Checking {executable}")
                result = subprocess.run([ldd_exe, executable], capture_output=True, text=True)
                all_dpendencies.append(result.stdout)
        return all_dpendencies
# ok, lets run it
if __name__ == "__main__":
    # all_dpendencies = []
    deps_libs = []
    # get the dependencies
    all_dpendencies = get_dependencies_all(scandir)
    # remove all containing /WINDOWS/
    #all_dpendencies = [dep for dep in all_dpendencies if "/WINDOWS/" not in dep]
    for dep in all_dpendencies:
        # split the line by spaces
        # print("dep is ----> ", dep)
        deps = dep.split() 
        # we have a new array
        # we remmove all where string contains /WINDOWS/ and build a new array
        for d in deps:
            # if NOT contains /WINDOWS/ BUT contains /uctr64/
            if "/WINDOWS/" not in d and "/ucrt64/" in d:
               # print("d is ----> ", d)
                # add to deps_libs if not already in
                if d not in deps_libs:
                    deps_libs.append(d)
    # now we remove all duplicates
    deps_libs = list(set(deps_libs)) 
    # now we have to convert the paths to the correct format
    # remove the leading / and replace / with \
    deps_libs = [d[1:].replace("/", "\\") for d in deps_libs]
    # now we have to ADD the leading C:\msys64\ to the paths using msys2_urcrt_root variable
    deps_libs = [msys2_urcrt_root + "\\" + d for d in deps_libs]
    print(home_dir)  
    # first we call windeployqt6.exe to deploy the qt libraries
    # windeployqt6 --libdir scandir --plugindir scandir --no-translations --compiler-runtime scandir
    subprocess.run([windeployqt6, "--libdir", scandir, "--plugindir", scandir, "--no-translations", "--compiler-runtime", scandir])
    # extra libs to be copied from 
    # now we have to copy the dependencies from msys2_urcrt_bin to scandir if not exists in scandir
    
    for dep in deps_libs:
        # check if the file exists in scandir
        dep_file = os.path.basename(dep)
        dep_path = os.path.join(scandir, dep_file)
        # first we check if the source file exists
        if not os.path.exists(dep):
            print(f"Source file {dep} does not exist")
            continue
        # if not exists copy it
        if not os.path.exists(dep_path):
            print(f"Copying {dep} to {scandir}")
            # copy the file
            shutil.copy(dep, scandir)
        else:
            print(f"{dep_file} already exists in {scandir}")
    # missing still
    # Qt6OpenGL.dll Qt6OpenGL.dll Qt6OpenGLWidgets.dll Qt6PrintSupport.dll Qt6Xml.dll Qt6SvgWidgets.dll Qt6Svg.dll libstdc++-6.dll libgcc_s_seh-1.dll libwinpthread-1.dll libclang-cpp.dll 
    missing_libs = [
        "Qt6OpenGL.dll", "Qt6OpenGLWidgets.dll", "Qt6PrintSupport.dll",
        "Qt6Xml.dll", "Qt6SvgWidgets.dll", "Qt6Svg.dll",
        "libstdc++-6.dll", "libgcc_s_seh-1.dll", "libwinpthread-1.dll", "libclang-cpp.dll"
    ]
    # Copy missing libs from msys2_urcrt_bin to scandir if not already present
    for lib in missing_libs:
        src_path = os.path.join(msys2_urcrt_bin, lib)
        dest_path = os.path.join(scandir, lib)
        # Check if the source file exists
        if not os.path.exists(src_path):
            print(f"Source file {src_path} does not exist")
            continue
        if not os.path.exists(dest_path):
            print(f"Copying {src_path} to {scandir}")
            shutil.copy(src_path, scandir)
        else:
            print(f"{lib} already exists in {scandir}")
    with open("dependencies.txt", "w") as f:
        for dep in deps_libs:
            f.write(dep + "\n")

    print("Finished")