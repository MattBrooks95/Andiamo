import sys #shell calls
import os  #get HOME value
import re  #check to see if Andiamo! is in the PATH

#get HOME value from the environment
home_path = os.environ['HOME'] 
print(home_path)

#append to the HOME path to get the location of the
#installer directory
installer_dir = home_path + "/Andiamo/ubuntu_install/"
print(installer_dir)

#append andiamo.desktop to the installer
file_path = installer_dir + "andiamo.desktop"

#if the file exists, leave
if os.path.isfile(file_path):
    print("File already exists")
    sys.exit(0)
else:
    print("File doesn't exist yet")

#if we are here, the local copy of the desktop file doesn't
#exist, and should be made and then exported to
#/usr/share/applications

local_desktop_file = open(file_path,"w")

#local_desktop_file.write("I want to go to Japan.")

#I think this needs to be here
local_desktop_file.write("[Desktop Entry]\n")

#I'm sure this could be changed dynamically somehow
local_desktop_file.write("Version=1.0\n")

#Let's Go! this sets the name of the application in Unity
local_desktop_file.write("Name=Andiamo!\n")

#Comment is what is a brief description of what
#the application does
local_desktop_file.write("Comment=Hauser-Feshbach made easier!\n")

#point to the binary in the Andiamo! install directory
bin_path = home_path + "/Andiamo/andiamo"
print(bin_path)
local_desktop_file.write("Exec="+bin_path+"\n")

#tell it what icon to use for the executable
pic_path = home_path + "/Andiamo/Assets/Images/andiamo.png"
local_desktop_file.write("Icon="+pic_path+"\n")

#GUI shouldn't need terminal commands, and if a user does
#they can run it from their terminal to supply args
local_desktop_file.write("Terminal=false\n")

#this is an application, not a shell script or something
local_desktop_file.write("Type=Application\n")

#this helps the dash organize executables
local_desktop_file.write("Categories=Utility;Application;\n")

#the file is now made
local_desktop_file.close()

global_file = "/usr/share/applications/andiamo.desktop"

#if the file exists, leave
if os.path.isfile(global_file):
    print("File already exists")
    sys.exit(0)
else:
    print("File doesn't exist yet")

#elsewise, copy it to usr/share/applications

#give this local copy execute permissions to test
#icon loading and make sure it runs when clicked
os.system("chmod +x "+file_path)

#copy the .desktop file to /usr/share/applications
print("Need sudo permissions to copy desktop file to applications.")
os.system("sudo cp "+file_path+" "+global_file)

#give that file executable permissions
os.system("sudo chmod +x "+global_file)

#if Andiamo's root folder is not in the PATH
path_env = os.environ['PATH']
print("Path before:\n"+path_env,end="\n")

#search the path string for the andiamo executable file path
andy_root_dir = home_path+"/Andiamo"
reg_pattern   = andy_root_dir
if re.search(reg_pattern,path_env):
    print("Andiamo's root dir is already in the path.")
else:
    print("Andiamo's root dir is not already in the path.")

    #append it to the PATH
    bash_prof = open(home_path+"/.bashrc","a")
    bash_prof.write("export PATH=$PATH:"+andy_root_dir)
    os.system("sudo ~/.bashrc")
    
print("You likely will need to restart your computer")
print(" in order for Andiamo! to be added to the PATH")
path_env = os.environ['PATH']
print("Path after:\n"+path_env,end="\n");








