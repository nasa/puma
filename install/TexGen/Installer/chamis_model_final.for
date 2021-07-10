C =============================================================================
C Abaqus UMAT Subroutine for Calculation of Localised Yarn Properties
C Copyright (C) 2016 Mikhail Matveev
C
C This program is free software; you can redistribute it and/or
C modify it under the terms of the GNU General Public License
C as published by the Free Software Foundation; either version 2
C of the License, or (at your option) any later version.
C
C This program is distributed in the hope that it will be useful,
C but WITHOUT ANY WARRANTY; without even the implied warranty of
C MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
C GNU General Public License for more details.
C
C You should have received a copy of the GNU General Public License
C along with this program; if not, write to the Free Software
C Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
C =============================================================================*/
C
      SUBROUTINE UMAT(STRESS,STATEV,DDSDDE,SSE,SPD,SCD,
     1 RPL,DDSDDT,DRPLDE,DRPLDT,STRAN,DSTRAN,
     2 TIME,DTIME,TEMP,DTEMP,PREDEF,DPRED,MATERL,NDI,NSHR,NTENS,
     3 NSTATV,PROPS,NPROPS,COORDS,DROT,PNEWDT,CELENT,
     4 DFGRD0,DFGRD1,NOEL,NPT,KSLAY,KSPT,KSTEP,KINC)
C
      INCLUDE 'ABA_PARAM.INC'
C
      CHARACTER*80 MATERL
      DIMENSION STRESS(NTENS),STATEV(NSTATV),
     1 DDSDDE(NTENS,NTENS),DDSDDT(NTENS),DRPLDE(NTENS),
     2 STRAN(NTENS),DSTRAN(NTENS),TIME(2),PREDEF(1),DPRED(1),
     3 PROPS(NPROPS),COORDS(3),DROT(3,3),
     4 DFGRD0(3,3),DFGRD1(3,3)
C
      DIMENSION SPR(3)
      PARAMETER (ONE=1.0D0,TWO=2.0D0,THREE=3.0D0,SIX=6.0D0)
      DATA NEWTON,TOLER/10,1.D-6/
C
C -----------------------------------------------------------
C     UMAT FOR ORTHTROPIC MATERIAL
C     CANNOT BE USED FOR PLANE STRESS
C -----------------------------------------------------------
C     Using Chamis microformulae
C     PROPS(1) - fibre's longitudinal modulus E1
C     PROPS(2) - fibre's transverse modulus E2
C     PROPS(3) - fibre's Poisson ratio nu12
C     PROPS(4) - fibre's Poisson ratio nu23
C     PROPS(5) - fibre's Poisson ratio G12
C     PROPS(6) - fibre's Poisson ratio G23
C     PROPS(7) - matrix Young's modulus
C     PROPS(8) - matrix Poisson ratio
C     STATEV(4) - LOCAL FIBRE VOLUME FRACTION (READ FROM *.ELD FILE)
C -----------------------------------------------------------
C
      IF (NDI.NE.3) THEN
         WRITE(6,1)
 1       FORMAT(//,30X,'***ERROR - THIS UMAT MAY ONLY BE USED FOR ',
     1          'ELEMENTS WITH THREE DIRECT STRESS COMPONENTS')
      ENDIF
C
      CALL UMAT_YARN(STRESS,STATEV,DDSDDE,SSE,SPD,SCD,
     1 RPL,DDSDDT,DRPLDE,DRPLDT,STRAN,DSTRAN,
     2 TIME,DTIME,TEMP,DTEMP,PREDEF,DPRED,MATERL,NDI,NSHR,NTENS,
     3 NSTATV,PROPS,NPROPS,COORDS,DROT,PNEWDT,CELENT,
     4 DFGRD0,DFGRD1,NOEL,NPT,KSLAY,KSPT,KSTEP,KINC)        
C
      RETURN
      END      
C
C -----------------------------------------------------------------
C
C     SUBROUTINE FOR YARN
C
C -----------------------------------------------------------------
C      
      subroutine UMAT_YARN(STRESS,STATEV,DDSDDE,SSE,SPD,SCD,
     1 RPL,DDSDDT,DRPLDE,DRPLDT,STRAN,DSTRAN,
     2 TIME,DTIME,TEMP,DTEMP,PREDEF,DPRED,MATERL,NDI,NSHR,NTENS,
     3 NSTATV,PROPS,NPROPS,COORDS,DROT,PNEWDT,CELENT,
     4 DFGRD0,DFGRD1,NOEL,NPT,KSLAY,KSPT,KSTEP,KINC)
C
      INCLUDE 'ABA_PARAM.INC'
C
      CHARACTER*80 MATERL
      DIMENSION STRESS(NTENS),STATEV(NSTATV),
     1 DDSDDE(NTENS,NTENS),DDSDDT(NTENS),DRPLDE(NTENS),
     2 STRAN(NTENS),DSTRAN(NTENS),TIME(2),PREDEF(1),DPRED(1),
     3 PROPS(NPROPS),COORDS(3),DROT(3,3),
     4 DFGRD0(3,3),DFGRD1(3,3)
C
      DIMENSION SPR(3), DSTRESS(6)
      INTEGER, DIMENSION(:), ALLOCATABLE :: seed
      INTEGER :: clock
      PARAMETER (ONE=1.0D0,TWO=2.0D0,THREE=3.0D0,SIX=6.0D0)
      DATA NEWTON,TOLER/10,1.D-6/
C
      Em = PROPS(7)
      Gm = Em/2.0/(1.0+PROPS(8))
      E_vf = statev(4)
C      
      E1=statev(4)*props(1) + Em*(1.0-statev(4))
      E2=Em/(1.0 - sqrt(E_vf) * (1.0-Em/PROPS(2)))
      E3=Em/(1.0 - sqrt(E_vf) * (1.0-Em/PROPS(2)))
      Enu12=E_vf*PROPS(3)+(1.0 - E_vf) * PROPS(8)
      Enu13=E_vf*PROPS(3)+(1.0 - E_vf) * PROPS(8)
      Enu23=E_vf * PROPS(4) + (1.0-E_vf)*(2.0*PROPS(8) - Enu12/E1*E2)
      G12=Gm / (1.0-sqrt(E_vf) * (1.0 - Gm/PROPS(5)))
      G13=Gm / (1.0-sqrt(E_vf) * (1.0 - Gm/PROPS(5)))
      G23=Gm / (1.0 - E_vf * (1.0 - Gm/PROPS(6)))
C
C ASSIGN ENGINEERING CONSTANTS
C
C
C RECALCULATE COMPLIANCE MATRIX TO STIFFNESS MATRIX
C CALCULATE DETERMINANT AND MAJOR MINORS OF COMPLIANCE MATRIX
C 
C        1    (A B B)
C  C = -----  (B C H)
C      DET(S) (B H C)
C
      DET = (1.0 + Enu23)*( (1.0 - Enu23)/E2 - TWO*Enu12**2/E1) /E1/E2
      A = (1.0 - Enu23**2)/E2/E2
      B = Enu12 * (1.0 + Enu23)/E1/E2
      C = 1.0/E1/E2 - (Enu12/E1)**2
      H = (Enu12/E1)**2 + Enu23/E1/E2
C
C ELASTIC STIFFNESS
C
      DDSDDE(1,1) = A/DET
      DDSDDE(2,1) = B/DET
      DDSDDE(3,1) = B/DET
      DDSDDE(1,2) = B/DET
      DDSDDE(2,2) = C/DET
      DDSDDE(3,2) = H/DET
      DDSDDE(1,3) = B/DET
      DDSDDE(2,3) = H/DET
      DDSDDE(3,3) = C/DET
      DDSDDE(4,4) = G12
      DDSDDE(5,5) = G13
      DDSDDE(6,6) = G23
C
C CALCULATE STRESS
C
      SINC = 0
      DO K1=1, NTENS
        stress(k1) = 0.0
        DO K2=1, NTENS
          SINC=stran(k2) + dstran(k2)
          STRESS(K1) = stress(k1) + DDSDDE(K1, K2)*SINC
        END DO
      END DO
C
      RETURN
      END
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC