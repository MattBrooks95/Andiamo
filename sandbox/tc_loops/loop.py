import array
import random
from datetime import datetime

def main():
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


  for c in range(6):
    temp_ETAB = 0.0
    temp_TTAB = 0.0
    nent_index = 0.0
    nent_lmax_label_temp = '{:10d}{:10d}'.format(NENT[c],LMAX[c])
    print(nent_lmax_label_temp)
    nent_lmax_label.append(str(nent_lmax_label_temp))
 
    for l_control in range(LMAX[c]+1):
      temp_ETAB = 0.0
      temp_TTAB = 0.0
      for n_control in range(NENT[c]):
        temp_ETAB = temp_ETAB + .97
        temp_TTAB = random.normalvariate(.6,.2)
        ETAB.append(temp_ETAB)
        TTAB.append(temp_TTAB)
        third_value.append(l_control)
        nent_lmax_label.append(" ")
    nent_lmax_label.pop()

  output_f = open("output.txt",'w')

  index = 0
  for e_control in ETAB:
    if not(nent_lmax_label[index] == " "):
      output_f.write(nent_lmax_label[index]+"\n")
    line_out = '{:10.2f}{:10.2f}{:5d}'.format(e_control,TTAB[index],third_value[index])
    output_f.write(line_out+"\n")
    index += 1




main()
