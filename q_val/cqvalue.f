       subroutine cqvalue(izp,iap,izt,iat,qn,qp,qd,qt,        
     >                          q3He,qalpha,tflag)

C      
C      This is a subroutine to calulate the possible decay channels in
C      a Hauser-Feshbach calculation
C
C      The Q-values are calculated with an outgoing
C
C      neutron 
C      proton
C      deuteron
C      triton 
C      3He 
C      alpha
C
C      January 31, 2018 T. N. Massey
C

       implicit none

       integer izp,iap,izt,iat
       real*8 qn,qp,qd,qt,q3He,qalpha
       real*8 eqn,eqp,eqd,eqt,eq3He,eqalpha
       integer nr,jflag,kflag,tflag
       integer izcmp,iacmp,iz3(2),ia3(2)
C      print *,"izp = ",izp
C      print *,"iap = ",iap
C      print *,"izt = ",izt
C      print *,"iat = ",iat

       nr = 2    ! normal two body reaction 
       jflag = 0 ! set up for best method calculation
       kflag = 0 ! set up for best method calculation
       tflag = 0 ! start value for error flag

       izcmp = izp + izt   ! compound Z
       iacmp = iap + iat   ! compound A

C      neutron out

       iz3(1) = izcmp
       ia3(1) = iacmp - 1
       iz3(2) = 0
       ia3(2) = 1

C      print *, "izcmp = ",izcmp
C      print *, "iacmp = ",iacmp
C      print *, "iz3 = ",iz3
C      print *, "ia3 = ",ia3

       call dqvalu(izt,iat,izp,iap,iz3,ia3,nr,jflag,kflag,
     >              qn,eqn)
C       print *,"nr  = ",nr
C       print *,"jflag = ",jflag
C       print *,"kflag = ",kflag
C       print *,"qn = ",qn
C       print *,"eqn = ",eqn
C  
        tflag = tflag + kflag

C      pronton out

       iz3(1) = izcmp - 1
       ia3(1) = iacmp - 1
       iz3(2) = 1
       ia3(2) = 1

       call dqvalu(izt,iat,izp,iap,iz3,ia3,nr,jflag,kflag,
     >              qp,eqp)
   
        tflag = tflag + kflag


C      deuteron out

       iz3(1) = izcmp - 1
       ia3(1) = iacmp - 2
       iz3(2) = 1
       ia3(2) = 2

       call dqvalu(izt,iat,izp,iap,iz3,ia3,nr,jflag,kflag,
     >              qd,eqd)
   
        tflag = tflag + kflag


C      trition out

       iz3(1) = izcmp - 1
       ia3(1) = iacmp - 3
       iz3(2) = 1
       ia3(2) = 3

       call dqvalu(izt,iat,izp,iap,iz3,ia3,nr,jflag,kflag,
     >      qt,eqt)

   
        tflag = tflag + kflag

C      3He out

       iz3(1) = izcmp - 2
       ia3(1) = iacmp - 3
       iz3(2) = 2
       ia3(2) = 3

       call dqvalu(izt,iat,izp,iap,iz3,ia3,nr,jflag,kflag,
     >              q3He,eq3He)

   
        tflag = tflag + kflag

C      alpha out

       iz3(1) = izcmp - 2
       ia3(1) = iacmp - 4
       iz3(2) = 2
       ia3(2) = 4

       call dqvalu(izt,iat,izp,iap,iz3,ia3,nr,jflag,kflag,
     >              qalpha,eqalpha)
   
        tflag = tflag + kflag


       return
       end
