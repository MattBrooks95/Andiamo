      subroutine dmass(iz,ia,iflag,jflag,kflag,value,error)

**    @(#)dmass.f	1.2 90/02/09 14:10:44  --  D. A. Resler
**
**    routine to provide mass information...
**
**    ------------------------------------------------------------------
**
**    iz & ia  -->  Z and A of nucleus
**
**    iflag = -3  -->  acknowledge whether nucleus is in the table
**            -2  -->  return binding energy (MeV)
**            -1  -->  return atomic mass excess (MeV)
**             0  -->  return atomic mass (u)
**             1  -->  return nuclear mass (u)
**
**    jflag = 0  -->  employ "best" method (if more than one possible)
**            1  -->  employ method 1
**            2  -->  employ method 2
**            etc...  (currently not used...)
**
**    kflag = 0  -->  no errors encountered
**            1  -->  nucleus not in table
**
**    value  -->  returned value (determined by 'iflag')
**
**    error  -->  error in the returned value
**
**    ------------------------------------------------------------------

      implicit none
C     implicit undefined (a-z)
      integer iz,ia,iflag,jflag,kflag,jza,nj,mj,i,iza,isave,
     >  mz,iabeg,iaend
      real*8 value,error,ex,dex,eb,deb

      include 'physical_constants'

**    arrays for holding the mass table...


      parameter (mj=3436)

      dimension jza(mj),ex(mj),dex(mj),eb(mj),deb(mj)

      parameter (mz=118)

      dimension iabeg(0:mz),iaend(0:mz)

C     common /amt12/ ex,dex,eb,deb,jza,iabeg,iaend
      common /amt16/ ex,dex,eb,deb,jza,iabeg,iaend

      nj = mj

      value = 0.0d0
      error = 0.0d0

**    special cases...

      if(iz.eq.0.and.ia.eq.0) goto 49

**    is the nucleus in the table???

      if(iz.lt.0        .or.iz.gt.mz       ) goto 35

      if(ia.lt.iabeg(iz).or.ia.gt.iaend(iz)) goto 35

      if(iflag.eq.-3) goto 49

**    determine where in the table...

      iza = iz*1000+ia

      do 30 i=1,nj

          if(iza.eq.jza(i)) then

              isave = i
              goto 40

          end if

   30 continue

   35 kflag = 1

      goto 50

**    binding energy (MeV)...

   40 if(iflag.eq.-2) then

          value =  eb(isave)
          error = deb(isave)

**    mass excess (MeV)...

      elseif(iflag.eq.-1) then

          value =  ex(isave)
          error = dex(isave)

**    atomic mass (u)...

      elseif(iflag.eq.0) then

          value = dble(ia)
     >          + ex(isave)/cf_u

          error = sqrt( dex(isave)**2
     >                + (ce_u*ex(isave)/cf_u)**2 ) / cf_u

**    nuclear mass (u)...

      elseif(iflag.gt.0) then

**    binding energy of electrons calculated from
**        Allard, J Phy Et Rad 9, 225 (1948)

          value = dble(ia)
     >          + ex(isave)/cf_u
     >          - pc_em*dble(iz)/cf_u
     >          + (15.73d-6)*dble(iz)**(7.0d0/3.0d0)/cf_u

          error = sqrt( dex(isave)**2
     >                + (ce_u*(-ex(isave)
     >                         +pc_em*dble(iz)
     >                         -(15.73d-6)*dble(iz)**(7.0d0/3.0d0)
     >                        )/cf_u)**2
     >                + (pe_em*dble(iz))**2
     >                + ((0.005d-6)*dble(iz)**(7.0d0/3.0d0))**2 ) / cf_u

      end if

   49 kflag = 0

   50 end
