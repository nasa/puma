/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2006 Martin Sherburn

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
=============================================================================*/

#pragma once

#include <ostream>
#include <iomanip>
#include <assert.h>


namespace TexGen
{ 
	using namespace std;

	/// Class to represent a matrix and perform various operations on it
	class CMatrix
	{
	public:
		CMatrix();
		CMatrix(int iHeight, int iWidth);
		CMatrix(const CMatrix &CopyMe);
		~CMatrix(void);

		void Initialise(int iHeight, int iWidth);
		void InitialiseIdentity(int iSize);

		CMatrix GetTranspose();
		CMatrix &GetSubMatrix(CMatrix &SubMatrix, int iRow, int iColumn) const;
		void SetSubMatrix( CMatrix &SubMatrix, int iRow, int iColumn );
		void ZeroMatrix();
		double &operator() (int i, int j);
		const double &operator() (int i, int j) const;
		const double GetValue(int i, int j) const;
		void SetValue(int i, int j, double dVal);

		CMatrix operator * (const CMatrix &RightMatrix) const;		// These functions should not be used where performance is critical
		CMatrix operator * (double dRight) const;					// removed because they are not efficient
		CMatrix operator + (const CMatrix &RightMatrix) const;		// Use EqualsMultiple functions instead or += operator
		CMatrix operator - (const CMatrix &RightMatrix) const;

		CMatrix &operator *= (double dRight);
		CMatrix &operator /= (double dRight);
		CMatrix &operator += (const CMatrix &RightMatrix);
		CMatrix &operator -= (const CMatrix &RightMatrix);
		CMatrix &operator = (const CMatrix &RightMatrix);
		double GetDeterminant() const;
		double GetInverse(CMatrix &Inverse) const;
		void GetSquareRoot(CMatrix &Root) const;
		void GetEigen(CMatrix &EigenVectors, CMatrix &EigenValues) const;
		void GetPolarDecomposition(CMatrix &U, CMatrix &P) const;
//		operator bool() const;
		bool Empty() const;
		int GetHeight() const;
		int GetWidth() const;
		void SwapRows(int iRow1, int iRow2);
		void SwapColumns(int iColumn1, int iColumn2);
		void DivideRow(int iRow, double dDivisor);
		void DivideColumn(int iColumn, double dDivisor);
		void SubtractRow(int iRow1, int iRow2, double dScale);
		void SubtractColumn(int iColumn1, int iColumn2, double dScale);
		void Identity();

		/// This function multiplies the left matrix with the right matrix
		/**
		Warning, LeftMatrix and RightMatrix parameters should not be equal to this
		*/
		CMatrix &EqualsMultiple(const CMatrix &LeftMatrix, const CMatrix &RightMatrix);
		/// This function multiplies the transpose of the left matrix with the right matrix
		/**
		Warning, LeftMatrix and RightMatrix parameters should not be equal to this
		*/
		CMatrix &EqualsTransposeMultiple(const CMatrix &LeftMatrix, const CMatrix &RightMatrix);
		/// This function multiplies the transpose of the left matrix with the right matrix
		/**
		Warning, LeftMatrix and RightMatrix parameters should not be equal to this
		*/
		CMatrix &EqualsMultipleTranspose(const CMatrix &LeftMatrix, const CMatrix &RightMatrix);

		void PrintMatrix(char szMatrixName[], ostream &Output = cout, int iWidth = 16, bool bScientific = true);

	protected:
		double GetInverseSlow(CMatrix &Inverse) const;
		int m_iWidth;
		int m_iHeight;
		double *m_dMatrix;
	};

	inline CMatrix::CMatrix()
	: m_iWidth(0)
	, m_iHeight(0)
	, m_dMatrix(NULL)
	{
	}

	inline CMatrix::CMatrix(int iHeight, int iWidth)
	: m_iWidth(iWidth)
	, m_iHeight(iHeight)
	{
		m_dMatrix = new double [m_iWidth*m_iHeight];
		ZeroMatrix();
	}

	inline CMatrix::CMatrix(const CMatrix &CopyMe)
	: m_iWidth(0)
	, m_iHeight(0)
	, m_dMatrix(NULL)
	{
		(*this) = CopyMe;
	}


	inline CMatrix::~CMatrix(void)
	{
		if (m_dMatrix)
			delete [] m_dMatrix;
	}

	inline CMatrix &CMatrix::operator = (const CMatrix &RightMatrix)
	{
		if (m_iHeight != RightMatrix.m_iHeight || m_iWidth != RightMatrix.m_iWidth)
		{
			Initialise(RightMatrix.m_iHeight, RightMatrix.m_iWidth);
		}
		memcpy(m_dMatrix, RightMatrix.m_dMatrix, m_iWidth*m_iHeight*sizeof(double));
		return *this;
	}

	inline void CMatrix::Initialise(int iHeight, int iWidth)
	{
		assert(iHeight*iWidth > 0);
		if (m_dMatrix)
			delete [] m_dMatrix;
		m_iWidth = iWidth;
		m_iHeight = iHeight;
		m_dMatrix = new double [m_iWidth*m_iHeight];
		ZeroMatrix();
	}

	inline void CMatrix::InitialiseIdentity(int iSize)
	{
		assert(iSize > 0);
		if (m_dMatrix)
			delete [] m_dMatrix;
		m_iWidth = iSize;
		m_iHeight = iSize;
		m_dMatrix = new double [iSize*iSize];
		Identity();
	}

/*	inline CMatrix::operator bool() const
	{
		return (m_iWidth && m_iHeight);
	}*/

	inline bool CMatrix::Empty() const
	{
		return (m_iWidth==0 || m_iHeight==0);
	}

	inline int CMatrix::GetHeight() const
	{
		return m_iHeight;
	}

	inline int CMatrix::GetWidth() const
	{
		return m_iWidth;
	}

	inline double &CMatrix::operator() (int i, int j)
	{
		assert(i >= 0 && i < m_iHeight && j >= 0 && j < m_iWidth);

		return m_dMatrix[i*m_iWidth+j];
	}

	inline const double &CMatrix::operator() (int i, int j) const
	{
		assert(i >= 0 && i < m_iHeight && j >= 0 && j < m_iWidth);

		return m_dMatrix[i*m_iWidth+j];
	}

	inline const double CMatrix::GetValue(int i, int j) const
	{
		assert(i >= 0 && i < m_iHeight && j >= 0 && j < m_iWidth);

		return m_dMatrix[i*m_iWidth+j];
	}

	inline void CMatrix::SetValue(int i, int j, double dVal)
	{
		assert(i >= 0 && i < m_iHeight && j >= 0 && j < m_iWidth);

		m_dMatrix[i*m_iWidth+j] = dVal;
	}


	inline void CMatrix::ZeroMatrix()
	{
		memset(m_dMatrix, 0, m_iHeight*m_iWidth*sizeof(double));
	}

	inline CMatrix CMatrix::GetTranspose()
	{
		CMatrix Transpose(m_iWidth, m_iHeight);
		int i, j;
		for (i=0; i<m_iHeight; ++i)
		{
			for (j=0; j<m_iWidth; ++j)
			{
				Transpose(j, i) = (*this)(i, j);
			}
		}
		return Transpose;
	}

	inline CMatrix CMatrix::operator * (const CMatrix &RightMatrix) const
	{
		assert(m_iWidth == RightMatrix.m_iHeight);
		CMatrix Multiplication(m_iHeight, RightMatrix.m_iWidth);
		int i, j, k;
		for (i=0; i<m_iHeight; ++i)
		{
			for (j=0; j<RightMatrix.m_iWidth; ++j)
			{
				for  (k=0; k<m_iWidth; ++k)
				{
					Multiplication(i, j) += (*this)(i, k) * RightMatrix(k, j);
				}
			}
		}
		return Multiplication;
	}

	inline CMatrix CMatrix::operator * (double dRight) const
	{
		CMatrix Multiplication(m_iHeight, m_iWidth);
		int i, j;
		for (i=0; i<m_iHeight; ++i)
		{
			for (j=0; j<m_iWidth; ++j)
			{
				Multiplication(i, j) = (*this)(i, j) * dRight;
			}
		}
		return Multiplication;
	}

	inline CMatrix CMatrix::operator + (const CMatrix &RightMatrix) const
	{
		assert(m_iWidth == RightMatrix.m_iWidth);
		assert(m_iHeight == RightMatrix.m_iHeight);
		CMatrix Addition(m_iHeight, m_iWidth);
		int i;
		for (i=0; i<m_iHeight*m_iWidth; ++i)
		{
			Addition.m_dMatrix[i] = m_dMatrix[i] + RightMatrix.m_dMatrix[i];
		}
		return Addition;
	}

	inline CMatrix CMatrix::operator - (const CMatrix &RightMatrix) const
	{
		assert(m_iWidth == RightMatrix.m_iWidth);
		assert(m_iHeight == RightMatrix.m_iHeight);
		CMatrix Addition(m_iHeight, m_iWidth);
		int i;
		for (i=0; i<m_iHeight*m_iWidth; ++i)
		{
			Addition.m_dMatrix[i] = m_dMatrix[i] - RightMatrix.m_dMatrix[i];
		}
		return Addition;
	}


	inline CMatrix &CMatrix::EqualsMultiple(const CMatrix &LeftMatrix, const CMatrix &RightMatrix)
	{
		assert(LeftMatrix.m_iWidth == RightMatrix.m_iHeight);
		assert(&LeftMatrix != this);
		assert(&RightMatrix != this);
		if (m_iHeight != LeftMatrix.m_iHeight || m_iWidth != RightMatrix.m_iWidth)
		{
			Initialise(LeftMatrix.m_iHeight, RightMatrix.m_iWidth);
		}
		else
		{
			ZeroMatrix();
		}
		int i, j, k;
		for (i=0; i<m_iHeight; ++i)
		{
			for (j=0; j<m_iWidth; ++j)
			{
				for  (k=0; k<LeftMatrix.m_iWidth; ++k)
				{
					(*this)(i, j) += LeftMatrix(i, k) * RightMatrix(k, j);
				}
			}
		}
		return *this;
	}

	inline CMatrix &CMatrix::EqualsTransposeMultiple(const CMatrix &LeftMatrix, const CMatrix &RightMatrix)
	{
		assert(LeftMatrix.m_iHeight == RightMatrix.m_iHeight);
		assert(&LeftMatrix != this);
		assert(&RightMatrix != this);
		if (m_iHeight != LeftMatrix.m_iWidth || m_iWidth != RightMatrix.m_iWidth)
		{
			Initialise(LeftMatrix.m_iWidth, RightMatrix.m_iWidth);
		}
		else
		{
			ZeroMatrix();
		}
		int i, j, k;
		for (i=0; i<m_iHeight; ++i)
		{
			for (j=0; j<m_iWidth; ++j)
			{
				for  (k=0; k<LeftMatrix.m_iHeight; ++k)
				{
					(*this)(i, j) += LeftMatrix(k, i) * RightMatrix(k, j);
				}
			}
		}
		return *this;
	}

	inline CMatrix &CMatrix::EqualsMultipleTranspose(const CMatrix &LeftMatrix, const CMatrix &RightMatrix)
	{
		assert(LeftMatrix.m_iWidth == RightMatrix.m_iWidth);
		assert(&LeftMatrix != this);
		assert(&RightMatrix != this);
		if (m_iHeight != LeftMatrix.m_iHeight || m_iWidth != RightMatrix.m_iHeight)
		{
			Initialise(LeftMatrix.m_iHeight, RightMatrix.m_iHeight);
		}
		else
		{
			ZeroMatrix();
		}
		int i, j, k;
		for (i=0; i<m_iHeight; ++i)
		{
			for (j=0; j<m_iWidth; ++j)
			{
				for (k=0; k<LeftMatrix.m_iWidth; ++k)
				{
					(*this)(i, j) += LeftMatrix(i, k) * RightMatrix(j, k);
				}
			}
		}
		return *this;
	}

	inline CMatrix &CMatrix::operator *= (double dRight)
	{
		int i, j;
		for (i=0; i<m_iHeight; ++i)
		{
			for (j=0; j<m_iWidth; ++j)
			{
				(*this)(i, j) *= dRight;
			}
		}
		return *this;
	}

	inline CMatrix &CMatrix::operator /= (double dRight)
	{
		int i, j;
		dRight = 1.0/dRight;
		for (i=0; i<m_iHeight; ++i)
		{
			for (j=0; j<m_iWidth; ++j)
			{
				(*this)(i, j) *= dRight;
			}
		}
		return *this;
	}

	inline CMatrix &CMatrix::operator += (const CMatrix &RightMatrix)
	{
		if (m_iWidth != RightMatrix.m_iWidth || m_iHeight != RightMatrix.m_iHeight)
		{
			assert(Empty());
			Initialise(RightMatrix.m_iHeight, RightMatrix.m_iWidth);
		}
		int i, j;
		for (i=0; i<m_iHeight; ++i)
		{
			for (j=0; j<m_iWidth; ++j)
			{
				(*this)(i, j) += RightMatrix(i, j);
			}
		}
		return *this;
	}

	inline CMatrix &CMatrix::operator -= (const CMatrix &RightMatrix)
	{
		if (m_iWidth != RightMatrix.m_iWidth || m_iHeight != RightMatrix.m_iHeight)
		{
			assert(Empty());
			Initialise(RightMatrix.m_iHeight, RightMatrix.m_iWidth);
		}
		int i, j;
		for (i=0; i<m_iHeight; ++i)
		{
			for (j=0; j<m_iWidth; ++j)
			{
				(*this)(i, j) -= RightMatrix(i, j);
			}
		}
		return *this;
	}

	inline void CMatrix::PrintMatrix(char szMatrixName[], ostream &Output, int iWidth, bool bScientific)
	{
		int i, j;
		Output << szMatrixName << endl;
		if (bScientific)
			Output << setiosflags(ios_base::scientific);
		for (i=0; i<m_iHeight; ++i)
		{
			for (j=0; j<m_iWidth; ++j)
			{
				Output << setw(iWidth) << setprecision(iWidth-9) << (*this)(i, j);
			}
			Output << endl;
		}
		if (bScientific)
			Output << resetiosflags(ios_base::scientific);
	}

	inline ostream &operator << (ostream &output, CMatrix &Matrix)
	{
		Matrix.PrintMatrix((char*)"", output);
		return output;
	}

	inline void CMatrix::GetSquareRoot(CMatrix &Root) const
	{
		assert(m_iWidth == m_iHeight);

		int i;
		const int n = m_iWidth;

		CMatrix EigenValues;
		CMatrix EigenVectors;
		CMatrix EigenVectorsInverse;
		GetEigen(EigenVectors, EigenValues);
		EigenVectors.GetInverse(EigenVectorsInverse);
		CMatrix A(n, n);
		for (i=0; i<n; ++i)
		{
			assert(EigenValues(0, i) >= 0);
			A(i, i) = sqrt(EigenValues(0, i));
		}
	//	CMatrix A = EigenVectorsInverse * (*this) * EigenVectors;
	//	for (i=0; i<n; ++i)
	//	{
	//		A(i, i) = sqrt(A(i, i));
	//	}
		CMatrix Temp;
		Temp.EqualsMultiple(EigenVectors, A);
		Root.EqualsMultiple(Temp, EigenVectorsInverse);
	}

	inline double CMatrix::GetDeterminant() const
	{
		assert(m_iWidth == m_iHeight);

		const int n = m_iWidth; // == m_iHeight
		if (n == 1)
			return (*this)(0, 0);
		if (n == 2)
		{
			return (*this)(0, 0) * (*this)(1, 1) - (*this)(1, 0) * (*this)(0, 1);
		}
		if (n == 3)
		{
			double d1, d2, d3;
			d1 = (*this)(1, 1) * (*this)(2, 2) - (*this)(2, 1) * (*this)(1, 2);
			d2 = (*this)(1, 2) * (*this)(2, 0) - (*this)(2, 2) * (*this)(1, 0);
			d3 = (*this)(1, 0) * (*this)(2, 1) - (*this)(2, 0) * (*this)(1, 1);
			return (*this)(0, 0) * d1 + (*this)(0, 1) * d2 + (*this)(0, 2) * d3;
		}

		// Don't care about the inverse but it returns the determinant
		CMatrix Inverse;
		return GetInverse(Inverse);

/*		int i, j=0;
		double det = 0;
		CMatrix SubMatrix;
		for (i=0; i<n; ++i)
		{
			GetSubMatrix(SubMatrix, i, j);
			if ((i+j)%2 == 0)
				det += (*this)(i, j)*SubMatrix.GetDeterminant();
			else
				det -= (*this)(i, j)*SubMatrix.GetDeterminant();
		}
		return det;*/
	}

	inline void CMatrix::Identity()
	{
		assert(m_iWidth == m_iHeight);

		const int n = m_iWidth; // == m_iHeight

		int i, j;

		for (i=0; i<n; ++i)
		{
			for (j=0; j<n; ++j)
			{
				(*this)(i, j) = (i == j);
			}
		}
	}

	inline double CMatrix::GetInverse(CMatrix &Inverse) const
	{
		assert(m_iWidth == m_iHeight);

		const int n = m_iWidth; // == m_iHeight

		if (Inverse.m_iHeight != n || Inverse.m_iWidth != n)
			Inverse.Initialise(n, n);

		if (n == 2)
		{
			double dCoef;
			double dDeterminant = GetDeterminant();

			assert(dDeterminant);

			dCoef = 1.0/dDeterminant;

			Inverse(0, 0) = dCoef*(*this)(1, 1);
			Inverse(1, 1) = dCoef*(*this)(0, 0);
			Inverse(1, 0) = -dCoef*(*this)(1, 0);
			Inverse(0, 1) = -dCoef*(*this)(0, 1);

			return dDeterminant;
		}
		if (n == 3)
		{
			double dCoef;
			double dDeterminant = GetDeterminant();

			assert(dDeterminant);

			dCoef = 1.0/dDeterminant;

			int i, j;
			
			static CMatrix SubMatrix(2, 2);

			for (i=0; i<3; ++i)
			{
				for (j=0; j<3; ++j)
				{
					SubMatrix(0, 0) = (*this)((i+1)%3, (j+1)%3);
					SubMatrix(1, 0) = (*this)((i+2)%3, (j+1)%3);
					SubMatrix(0, 1) = (*this)((i+1)%3, (j+2)%3);
					SubMatrix(1, 1) = (*this)((i+2)%3, (j+2)%3);

					Inverse(j, i) = dCoef*SubMatrix.GetDeterminant();
				}
			}

			return dDeterminant;
		}

		CMatrix A = *this;

		int i, j, k;
		CMatrix b(n,n);	 // Scale factor and work array
		vector<double> scale(n);
		vector<int> index(n);

		//* Matrix b is initialized to the identity matrix
		b.Identity();

		//* Set scale factor, scale(i) = max( |a(i,j)| ), for each row
		for( i=0; i<n; i++ )
		{
			index[i] = i;			  // Initialize row index list
			double scalemax = 0.;
			for( j=0; j<n; j++ ) 
				scalemax = (scalemax > fabs(A(i,j))) ? scalemax : fabs(A(i,j));
			scale[i] = scalemax;
		}

		//* Loop over rows k = 1, ..., (n-1)
		int signDet = 1;
		for( k=0; k<n-1; k++ )
		{
			//* Select pivot row from max( |a(j,k)/s(j)| )
			double ratiomax = 0.0;
			int jPivot = k;
			for( i=k; i<n; i++ )
			{
				double ratio = fabs(A(index[i],k))/scale[index[i]];
				if( ratio > ratiomax )
				{
					jPivot=i;
					ratiomax = ratio;
				}
			}
			//* Perform pivoting using row index list
			int indexJ = index[k];
			if( jPivot != k )
			{	          // Pivot
				indexJ = index[jPivot];
				index[jPivot] = index[k];   // Swap index jPivot and k
				index[k] = indexJ;
				signDet *= -1;			  // Flip sign of determinant
			}
			//* Perform forward elimination
			for( i=k+1; i<n; i++ )
			{
				double coeff = A(index[i],k)/A(indexJ,k);
				for( j=k+1; j<n; j++ )
					A(index[i],j) -= coeff*A(indexJ,j);
				A(index[i],k) = coeff;
				for( j=0; j<n; j++ ) 
					b(index[i],j) -= A(index[i],k)*b(indexJ,j);
			}
		}
		//* Compute determinant as product of diagonal elements
		double determ = signDet;	   // Sign of determinant
		for( i=0; i<n; i++ )
			determ *= A(index[i],i);

		//* Perform backsubstitution
		for( k=0; k<n; k++ ) 
		{
			Inverse(n-1,k) = b(index[n-1],k)/A(index[n-1],n-1);
			for( i=n-2; i>=0; i--)
			{
				double sum = b(index[i],k);
				for( j=i; j<n; j++ )
					sum -= A(index[i],j)*Inverse(j,k);
				Inverse(i,k) = sum/A(index[i],i);
			}
		}

		return determ;

//		return GetInverseSlow(Inverse);
	}

	inline double CMatrix::GetInverseSlow(CMatrix &Inverse) const
	{
		assert(m_iWidth == m_iHeight);

		const int n = m_iWidth; // == m_iHeight

		if (Inverse.m_iHeight != n || Inverse.m_iWidth != n)
			Inverse.Initialise(n, n);

		double dCoef;
		double dDeterminant = GetDeterminant();

		assert(dDeterminant);

		dCoef = 1.0/dDeterminant;

		int i, j;
		
		CMatrix SubMatrix;

		for (i=0; i<n; ++i)
		{
			for (j=0; j<n; ++j)
			{
				GetSubMatrix(SubMatrix, i, j);
				Inverse(j, i) = dCoef*SubMatrix.GetDeterminant();
				if ((i+j)%2)
					Inverse(j, i) *= -1;
			}
		}

		return dDeterminant;
	}

	inline void CMatrix::SwapRows(int iRow1, int iRow2)
	{
		assert(iRow1 >= 0 && iRow1 < m_iHeight);
		assert(iRow2 >= 0 && iRow2 < m_iHeight);
		int i;
		double dTemp;
		for (i=0; i<m_iWidth; ++i)
		{
			dTemp = (*this)(iRow1, i);
			(*this)(iRow1, i) = (*this)(iRow2, i);
			(*this)(iRow2, i) = dTemp;
		}
	//	PrintMatrix();
	//	cout << endl;
	}

	inline void CMatrix::SwapColumns(int iColumn1, int iColumn2)
	{
		assert(iColumn1 >= 0 && iColumn1 < m_iWidth);
		assert(iColumn2 >= 0 && iColumn2 < m_iWidth);
		int i;
		double dTemp;
		for (i=0; i<m_iHeight; ++i)
		{
			dTemp = (*this)(i, iColumn1);
			(*this)(i, iColumn1) = (*this)(i, iColumn2);
			(*this)(i, iColumn2) = dTemp;
		}
	//	PrintMatrix();
	//	cout << endl;
	}

	inline void CMatrix::DivideRow(int iRow, double dDivisor)
	{
		assert(iRow >= 0 && iRow < m_iHeight);
		int i;
		for (i=0; i<m_iWidth; ++i)
		{
			(*this)(iRow, i) /= dDivisor;
		}
	//	PrintMatrix();
	//	cout << endl;
	}

	inline void CMatrix::DivideColumn(int iColumn, double dDivisor)
	{
		assert(iColumn >= 0 && iColumn < m_iWidth);
		int i;
		for (i=0; i<m_iHeight; ++i)
		{
			(*this)(i, iColumn) /= dDivisor;
		}
	//	PrintMatrix();
	//	cout << endl;
	}

	inline void CMatrix::SubtractRow(int iRow1, int iRow2, double dScale)
	{
		assert(iRow1 >= 0 && iRow1 < m_iHeight);
		assert(iRow2 >= 0 && iRow2 < m_iHeight);
		int i;
		for (i=0; i<m_iWidth; ++i)
		{
			(*this)(iRow1, i) -= (*this)(iRow2, i) * dScale;
		}
	//	PrintMatrix();
	//	cout << endl;
	}

	inline void CMatrix::SubtractColumn(int iColumn1, int iColumn2, double dScale)
	{
		assert(iColumn1 >= 0 && iColumn1 < m_iWidth);
		assert(iColumn2 >= 0 && iColumn2 < m_iWidth);
		int i;
		for (i=0; i<m_iHeight; ++i)
		{
			(*this)(i, iColumn1) -= (*this)(i, iColumn2) * dScale;
		}
	//	PrintMatrix();
	//	cout << endl;
	}

	inline CMatrix &CMatrix::GetSubMatrix(CMatrix &SubMatrix, int iRow, int iColumn) const
	{
		if (SubMatrix.m_iWidth != m_iWidth-1 && SubMatrix.m_iHeight != m_iHeight-1)
			SubMatrix.Initialise(m_iHeight-1, m_iWidth-1);

		int i, j, k, l;

		for (i=0, k=0; i<m_iWidth; ++i)
		{
			if (i == iColumn)
				continue;
			for (j=0, l=0; j<m_iHeight; ++j)
			{
				if (j == iRow)
					continue;
				SubMatrix(l, k) = (*this)(j, i);
				++l;
			}
			++k;
		}

		return SubMatrix;
	}

	inline void CMatrix::GetPolarDecomposition(CMatrix &U, CMatrix &P) const
	{
		assert(m_iWidth == m_iHeight);
		CMatrix PSquared, PInverse;
		PSquared.EqualsTransposeMultiple(*this, *this);
		PSquared.GetSquareRoot(P);
		P.GetInverse(PInverse);
		U.EqualsMultiple(*this, PInverse);
	}

	inline void CMatrix::GetEigen(CMatrix &EigenVectors, CMatrix &EigenValues) const
	{
		assert(m_iWidth == m_iHeight);

		const int n = m_iWidth;

		// WORKS ONLY FOR SYMETRIC MATRICES
		bool bSymetric = true;
		int i, j;
		for (i=0; i<n && bSymetric; ++i)
		{
			for (j=i+1; j<n && bSymetric; ++j)
			{
				if (fabs((*this)(i, j) - (*this)(j, i)) > 1e-6)
					bSymetric = false;
			}
		}
		assert(bSymetric);
		if (bSymetric)
		{
			if (EigenValues.m_iHeight != 1 || EigenValues.m_iWidth != n)
				EigenValues.Initialise(1, n);

			double *e = new double[n];

			int i, j, k, l;

			EigenVectors = (*this);

			//  This is derived from the Algol procedures tred2 by
			//  Bowdler, Martin, Reinsch, and Wilkinson, Handbook for
			//  Auto. Comp., Vol.ii-Linear Algebra, and the corresponding
			//  Fortran subroutine in EISPACK.
			for (j = 0; j < n; j++)
			{
				EigenValues(0, j) = EigenVectors(n - 1, j);
			}

			// Householder reduction to tridiagonal form.
			
			for (i = n - 1; i > 0; i--)
			{
				// Scale to avoid under/overflow.
				
				double scale = 0.0;
				double h = 0.0;
				for (k = 0; k < i; k++)
				{
					scale = scale + fabs(EigenValues(0, k));
				}
				if (scale == 0.0)
				{
					e[i] = EigenValues(0, i - 1);
					for (j = 0; j < i; j++)
					{
						EigenValues(0, j) = EigenVectors(i - 1, j);
						EigenVectors(i, j) = 0.0;
						EigenVectors(j, i) = 0.0;
					}
				}
				else
				{
					// Generate Householder vector.
					
					for (k = 0; k < i; k++)
					{
						EigenValues(0, k) /= scale;
						h += EigenValues(0, k) * EigenValues(0, k);
					}
					double f = EigenValues(0, i - 1);
					double g = sqrt(h);
					if (f > 0)
					{
						g = - g;
					}
					e[i] = scale * g;
					h = h - f * g;
					EigenValues(0, i - 1) = f - g;
					for (j = 0; j < i; j++)
					{
						e[j] = 0.0;
					}
					
					// Apply similarity transformation to remaining columns.
					
					for (j = 0; j < i; j++)
					{
						f = EigenValues(0, j);
						EigenVectors(j, i) = f;
						g = e[j] + EigenVectors(j, j) * f;
						for (k = j + 1; k <= i - 1; k++)
						{
							g += EigenVectors(k, j) * EigenValues(0, k);
							e[k] += EigenVectors(k, j) * f;
						}
						e[j] = g;
					}
					f = 0.0;
					for (j = 0; j < i; j++)
					{
						e[j] /= h;
						f += e[j] * EigenValues(0, j);
					}
					double hh = f / (h + h);
					for (j = 0; j < i; j++)
					{
						e[j] -= hh * EigenValues(0, j);
					}
					for (j = 0; j < i; j++)
					{
						f = EigenValues(0, j);
						g = e[j];
						for (k = j; k <= i - 1; k++)
						{
							EigenVectors(k, j) -= (f * e[k] + g * EigenValues(0, k));
						}
						EigenValues(0, j) = EigenVectors(i - 1, j);
						EigenVectors(i, j) = 0.0;
					}
				}
				EigenValues(0, i) = h;
			}

			// Accumulate transformations.
					
			for (i = 0; i < n - 1; i++)
			{
				EigenVectors(n - 1, i) = EigenVectors(i, i);
				EigenVectors(i, i) = 1.0;
				double h = EigenValues(0, i + 1);
				if (h != 0.0)
				{
					for (k = 0; k <= i; k++)
					{
						EigenValues(0, k) = EigenVectors(k, i + 1) / h;
					}
					for (j = 0; j <= i; j++)
					{
						double g = 0.0;
						for (k = 0; k <= i; k++)
						{
							g += EigenVectors(k, i + 1) * EigenVectors(k, j);
						}
						for (k = 0; k <= i; k++)
						{
							EigenVectors(k, j) -= g * EigenValues(0, k);
						}
					}
				}
				for (k = 0; k <= i; k++)
				{
					EigenVectors(k, i + 1) = 0.0;
				}
			}
			for (j = 0; j < n; j++)
			{
				EigenValues(0, j) = EigenVectors(n - 1, j);
				EigenVectors(n - 1, j) = 0.0;
			}
			EigenVectors(n - 1, n - 1) = 1.0;
			e[0] = 0.0;

			for (i = 1; i < n; i++)
			{
				e[i - 1] = e[i];
			}
			e[n - 1] = 0.0;
			
			double f = 0.0;
			double tst1 = 0.0;
			double eps = 2E-52;
			for (l = 0; l < n; l++)
			{
				// Find small subdiagonal element
				if (fabs(EigenValues(0, l)) + fabs(e[l]) > tst1)
					tst1 = fabs(EigenValues(0, l)) + fabs(e[l]);

				int m = l;
				while (m < n)
				{
					if (fabs(e[m]) <= eps * tst1)
					{
						break;
					}
					m++;
				}
				
				// If m == l, EigenValues(0, l) is an eigenvalue,
				// otherwise, iterate.
				
				if (m > l)
				{
					int iter = 0;
					do 
					{
						iter = iter + 1; // (Could check iteration count here.)
						
						// Compute implicit shift

						double g = EigenValues(0, l);
						double p = (EigenValues(0, l + 1) - g) / (2.0 * e[l]);
		//				double r = Maths.Hypot(p, 1.0);
						double r = sqrt(p*p+1);
						if (p < 0)
						{
							r = - r;
						}
						EigenValues(0, l) = e[l] / (p + r);
						EigenValues(0, l + 1) = e[l] * (p + r);
						double dl1 = EigenValues(0, l + 1);
						double h = g - EigenValues(0, l);
						for (i = l + 2; i < n; i++)
						{
							EigenValues(0, i) -= h;
						}
						f = f + h;
						
						// Implicit QL transformation.
						
						p = EigenValues(0, m);
						double c = 1.0;
						double c2 = c;
						double c3 = c;
						double el1 = e[l + 1];
						double s = 0.0;
						double s2 = 0.0;
						for (i = m - 1; i >= l; i--)
						{
							c3 = c2;
							c2 = c;
							s2 = s;
							g = c * e[i];
							h = c * p;
		//					r = Maths.Hypot(p, e[i]);
							r = sqrt(p*p+e[i]*e[i]);
							e[i + 1] = s * r;
							s = e[i] / r;
							c = p / r;
							p = c * EigenValues(0, i) - s * g;
							EigenValues(0, i + 1) = h + s * (c * g + s * EigenValues(0, i));
							
							// Accumulate transformation.
							
							for (k = 0; k < n; k++)
							{
								h = EigenVectors(k, i + 1);
								EigenVectors(k, i + 1) = s * EigenVectors(k, i) + c * h;
								EigenVectors(k, i) = c * EigenVectors(k, i) - s * h;
							}
						}
						p = (- s) * s2 * c3 * el1 * e[l] / dl1;
						e[l] = s * p;
						EigenValues(0, l) = c * p;
						
						// Check for convergence.
					}
					while (fabs(e[l]) > eps * tst1);
				}
				EigenValues(0, l) = EigenValues(0, l) + f;
				e[l] = 0.0;
			}

			// Sort eigenvalues and corresponding vectors.

			for (i = 0; i < n - 1; i++)
			{
				int k = i;
				double p = EigenValues(0, i);
				for (j = i + 1; j < n; j++)
				{
					if (EigenValues(0, j) < p)
					{
						k = j;
						p = EigenValues(0, j);
					}
				}
				if (k != i)
				{
					EigenValues(0, k) = EigenValues(0, i);
					EigenValues(0, i) = p;
					for (j = 0; j < n; j++)
					{
						p = EigenVectors(j, i);
						EigenVectors(j, i) = EigenVectors(j, k);
						EigenVectors(j, k) = p;
					}
				}
			}
			delete [] e;
		}
	}

	inline void CMatrix::SetSubMatrix(TexGen::CMatrix &SubMatrix, int iRow, int iColumn)
	{
		if ( (iColumn + SubMatrix.m_iWidth) > m_iWidth || (iRow + SubMatrix.m_iHeight) > m_iHeight )
			return;
		for ( int i = 0; i < SubMatrix.m_iHeight; ++i )
		{
			for ( int j = 0; j < SubMatrix.m_iWidth; ++j )
			{
				(*this).SetValue( iRow+i, iColumn+j, SubMatrix.GetValue( i, j ) );
			}
		}
	}
};	// namespace TexGen










