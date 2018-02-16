      subroutine dqvalu(iz1,ia1,iz2,ia2,iz3,ia3,nr,jflag,kflag,
     >  value,error)

**    @(#)dqvalu.f	1.1 90/02/09 09:21:24  --  D. A. Resler
**
**    routine to provide Q-value information for two-body reactions...
**
**    ------------------------------------------------------------------
**
**    iz1 & ia1  -->  Z and A of target nucleus
**    iz2 & ia2  -->  Z and A of projectile nucleus
**    iz3 & ia3  -->  Z and A of outgoing nuclei
**
**    nr  -->  number of outgoing nuclei
**
**    jflag = 0  -->  employ best method (if more than one possible)
**            1  -->  employ method 1
**            2  -->  employ method 2
**            etc...  (currently not used...)
**
**    kflag = 0  -->  no errors encountered
**            1  -->  error in obtaining mass of target nucleus
**            2  -->  error in obtaining mass of projectile nucleus
**            3  -->  error in obtaining mass of outgoing nucleus 1
**            4  -->  error in obtaining mass of outgoing nucleus 2
**            etc...
**
**    value  -->  Q-value (MeV)
**
**    error  -->  error in Q-value (MeV)
**
**    ------------------------------------------------------------------

      implicit none
C     implicit undefined (a-z)
      integer iz1,ia1,iz2,ia2,iz3,ia3,nr,jflag,kflag,i
      real*8 value,value1,value2,value3,valsum,
     >       error,error1,error2,error3,errsum

      dimension iz3(nr),ia3(nr)

C      include '/usr/user/sc/sf/pc/physical_constants'

      include 'physical_constants'

**    target nucleus...

      call dmass(iz1,ia1,1,jflag,kflag,value1,error1)


      

      if(kflag.ne.0) then

          kflag = 1
          goto 20

      end if

      valsum = value1
      errsum = error1*error1

**    projectile nucleus...

      call dmass(iz2,ia2,1,jflag,kflag,value2,error2)

      if(kflag.ne.0) then

          kflag = 2
          goto 20

      end if

      valsum = valsum+value2
      errsum = errsum+error2*error2

**    loop through outgoing nuclei...

      do 10 i=1,nr

          call dmass(iz3(i),ia3(i),1,jflag,kflag,value3,error3)

          if(kflag.ne.0) then

              kflag = i+2
              goto 20

          end if

          valsum = valsum-value3
          errsum = errsum+error3*error3

   10 continue

      value = valsum*cf_u

      error = sqrt(errsum*cf_u*cf_u+ce_u*ce_u*valsum*valsum)

   20 end
