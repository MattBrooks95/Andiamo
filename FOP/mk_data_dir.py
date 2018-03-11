import math
import random
import os
import sys
import proj_types

print("And where does the newborn go from here?")

#figure out the path to the home directory
home = os.path.expanduser("~")
print("HOME:"+home)

#assuming Andiamo is installed under /home/[user],
#craft a path the project's FOP directory
path = home + "/Andiamo/FOP"
print("PATH TO DIR:"+path)

#create the path for directory of data
#though here, we will be making a dummy directory

path_to_data = path
path_to_data += "/OpticalModels"


print("PATH TO DIR TO MAKE:"+path_to_data)

#loop over possible beams, 1 - 6
# with 1 = neutron, 2 = proton, 3 = deuteron
# 4 = triton, 5 = helium-3, 6 = alpha
projectiles = {"neutron " : proj_types.neutron(),
               "proton"  : proj_types.proton(),
               "deuteron": proj_types.deuteron(),
               "triton"  : proj_types.triton(),
               "helium_3": proj_types.helium_3(),
               "alpha"   : proj_types.alpha()}

#here we're going to loop over each of the projectile types
for particle in projectiles:

    print(particle,end=" ")
    print(projectiles[particle],end=" ")

    #zero padding of string representations of
    #integers:
    #stackoverflow.com/questions/339007/nicest-way-to-pad-zeroes-to-string
    proj_A = str(projectiles[particle].a)
    proj_A = proj_A.zfill(3)

    proj_Z = str(projectiles[particle].z)
    proj_Z = proj_Z.zfill(3)

    #print("PADDED A:"+padded_A)
    #print("PADDED Z:"+padded_Z)
    #wait = input()

    #projectile_path now is a string that represents the path
    #to this iteration of the loop's projectile directory
    projectile_path = path_to_data + "/" + proj_Z + "_" + proj_A + "/"
    print(projectile_path)

    #create a directory where we will put the data files
    os.mkdir(projectile_path)


#big_comment = '''
    #this is a dummy directory we're making,
    #so I'll just loop over all possible elements
    #as targets
    for targ_element in range (1, 119):

        #put the projectile info in the path again,
        #followed by Z for the given element
        element_path = projectile_path + proj_Z + "_" + proj_A
        element_path += "_"+str(targ_element).zfill(3)

        #the path is now complete all the way up until the
        #A value for the target
        print(element_path)
        print(targ_element,end=" ")

        #I don't know how to get a list of all of the possible
        #stable/used A values for these, so I'm going to assume that
        #A goes from Z to Z * 1.5
        stop = math.floor(targ_element * 1.5)
        print("Maximum A: "+str(stop))

        #this loop crafts the bogus A values to finish the path
        for bogus_A in range (targ_element,stop):

            #create the final path for this file
            file_path = element_path + "_" + str(bogus_A).zfill(3)
            print(file_path)

            #opening a file creates it
            file_stream = open(file_path,"w")

            #craft the header line
            header = proj_Z + " " + proj_A + " " + str(targ_element).zfill(3)
            header += " " + str(bogus_A).zfill(3) + "\n"
            file_stream.write(header)

            fake_doubles = []
            for fake_val in range (0,20):
                fake_doubles.append(str(random.random()))

            data = ""
            for double in fake_doubles:
                data += str(double).zfill(3) + " "
            data += "\n"

            file_stream.write(data)

        #end loop over each element's possible A values

    #end target_element for loop

#end proj_element for loop'''






