      subroutine desep(iz1,ia1,iz2,ia2,nr,jflag,kflag,value,error)

**    @(#)desep.f	1.1 90/02/09 09:21:22  --  D. A. Resler
**
**    routine to provide energy separation information...
**
**    ------------------------------------------------------------------
**
**    iz1 & ia1  -->  Z and A of compound nucleus
**    iz2 & ia2  -->  Z and A of constituent nuclei
**
**    nr  -->  number of constituent nuclei
**
**    jflag = 0  -->  employ "best" method (if more than one possible)
**            1  -->  employ method 1
**            2  -->  employ method 2
**            etc...  (currently not used...)
**
**    kflag = 0  -->  no errors encountered
**            1  -->  error in obtaining mass of compound nucleus
**            2  -->  error in obtaining mass of constituent nucleus 1
**            3  -->  error in obtaining mass of constituent nucleus 2
**            etc...
**
**    value  -->  separation energy (MeV)
**
**    error  -->  error in the separation energy (MeV)
**
**    ------------------------------------------------------------------

      implicit none
C     implicit undefined (a-z)
      integer iz1,ia1,iz2,ia2,nr,jflag,kflag,i
      real*8 value,value1,value2,valsum,
     >       error,error1,error2,errsum

      dimension iz2(nr),ia2(nr)

C     include '/home/sc/sf/pc/physical_constants'
      include 'physical_constants'

**    compound nucleus...

      call dmass(iz1,ia1,1,jflag,kflag,value1,error1)
C     print *,"Z =",iz1,"A=",ia1,"mass =",value1
      if(kflag.ne.0) then

          kflag = 1
          goto 20

      end if

      valsum = -value1
      errsum = error1*error1

**    loop through constituent nuclei...

      do 10 i=1,nr

          call dmass(iz2(i),ia2(i),1,jflag,kflag,value2,error2)
C     print *,"Z =",iz2(i),"A=",ia2(i),"mass =",value2

          if(kflag.ne.0) then

              kflag = i+1
              goto 20

          end if

          valsum = valsum+value2
          errsum = errsum+error2*error2

   10 continue

      value = valsum*cf_u

      error = sqrt(errsum*cf_u*cf_u+ce_u*ce_u*valsum*valsum)

   20 end
