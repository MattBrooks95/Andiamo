import array
import random
import sys
from datetime import datetime

def main():

  ##############################################################################################################
  #this block allows for the output file path to be given as an argument to the script
  if len(sys.argv) > 1:
    output_file = sys.argv[1]
  else:#if one is not defined it defaults to this
    output_file = "output.txt"
  ##############################################################################################################

  random.seed(datetime.now())

  NENT = array.array('I')
  LMAX = array.array('I')

  ETAB = array.array('f')
  TTAB = array.array('f')

  third_value = array.array('I')

  nent_lmax_label = []


  temp_num = 0
  for i in range(6):
    temp_num = int(input("Please enter NENT value:"))
    NENT.append(temp_num)
    print("\n")

  for j in range(6):
    temp_num = int(input("Please enter LMAX value:"))
    LMAX.append(temp_num)
    print("\n")


  for c in range(6):#run for LMAX's size
    temp_ETAB = 0.0
    temp_TTAB = 0.0
    nent_index = 0.0
    nent_lmax_label_temp = '{:10d}{:10d}'.format(NENT[c],LMAX[c]) #push the labels into the arrays to be printed l8er
    print(nent_lmax_label_temp)
    nent_lmax_label.append(str(nent_lmax_label_temp))
    for l_control in range(LMAX[c]+1):#loop over each lmax value
      temp_ETAB = 0.0
      temp_TTAB = 0.0
      for n_control in range(NENT[c]):#generating lmax things
        temp_ETAB = temp_ETAB + .97
        temp_TTAB = random.normalvariate(.6,.2)
        if temp_ETAB < 0:#make sure we only get positive values
          temp_ETAB *= -1
        if temp_TTAB < 0:
          temp_TTAB *= -1
        ETAB.append(temp_ETAB)
        TTAB.append(temp_TTAB)
        third_value.append(l_control)

  output_f = open(output_file,'w')

  for c in range(6):
    output_f.write(nent_lmax_label[c] + "\n")
    for l_control in range(LMAX[c]+1):
      for n_control in range(NENT[c]):
        temp_ETAB = '{:10.2f}'.format(ETAB[n_control])
        temp_TTAB = '{:10.2f}'.format(TTAB[n_control])
        temp_third_value = '{:5d}'.format(l_control)
        output_f.write(temp_ETAB + temp_TTAB + temp_third_value+"\n")




main()










