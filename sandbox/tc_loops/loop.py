import array
import random

def main():
  NENT = array.array('I')
  LMAX = array.array('I')

  ETAB = array.array('f')
  TTAB = array.array('f')

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
    for l_control in range(LMAX[c]):
      for n_control in range(NENT[c]):
        temp_ETAB = temp_ETAB + .97
        temp_TTAB = random.normalvariate(.6,.2)
        ETAB.append(temp_ETAB)
        TTAB.append(temp_TTAB)

  output_f = open("output.txt",'w')



  index = 0
  for e_control in ETAB:
    ETAB_out = format(
    output_f.write(format("%10.2f",str(e_control)) + " " + format("%10.2f",str(TTAB[index])) + "\n")





main()
