       program hf_qvalue

       implicit none
       integer nr,k,k1
       integer iap,izp,iat,izt,kflag
       integer iout
       real*8 value,error
       character*4 charAp,charZp
       character*4 charAt,charZt
       integer argno,tflag
        real*8    qn,qp,qd,qt,q3He,qalpha

C      calculation for 183W
C       ia = 183
C       iz = 74
       nr = 0
       iout = 10
       tflag = 0
       argno = iargc()
C      print *,argno
        if(argno.lt.4)then
       print *,"numbor of arguments = ",argno
       Print *,' hf_qvalue <a> <z> <A> <Z>'
       Print *,' '
       Print *,"<a> is Atomic Mass of the projectile"
       Print *,' '
       Print *,"<z> is the Z of the projectile"
       Print *,' '
       Print *,"<A> is Atomic Mass of the target nucleus"
       Print *,' '
       Print *,"<Z> is the Z of the target nucleus"
       Print *,' '
       Print *,' '
        STOP 'Not enough data in command string'
        endif
        call getarg(1,charAp)
        print *,"Projectile A = ",charAp
             read(charAp,*)iAp

             call getarg(2,charZp)
             print *,"projectile Z = ",charZp
             read(charZp,*)iZp

        call getarg(3,charAt)
             print *,"Target A = ",charAt
             read(charAt,*)iAt

             call getarg(4,charZt)
             print *,"target Z = ",charZt
             read(charZt,*)iZt

       open(10,file="hf_qvalue.out",status="Unknown")

        call cqvalue(izp,iap,izt,iat,qn,qp,qd,qt,q3He,qalpha,tflag)
        write(10,600)izp,iap,izt,iat

600     format(1x,"Projectile Z = ",I4," A = ",I4,
     >       1x,"Target     Z = ",I4," A = ",I4)
        write(10,700)qn,qp,qd,qt,q3He,qalpha

700     format(1x,"neutron out =  ",F15.7/
     <         1x,"proton out =   ",F15.7/
     <         1x,"deuteron out = ",F15.7/
     <         1x,"triton out =   ",F15.7/
     <         1x,"3He out =      ",F15.7/
     <         1x,"alpha out =    ",F15.7/)
        close(10) 
        end
