//-*- mode: c++ -*-
/// \file matrix_mn.hh
/// \brief General matrix operations.

#ifndef CGMATH_MATRIX_MN_HH
#define CGMATH_MATRIX_MN_HH

#include <cstdlib>
#include <stdexcept>
#include <limits>

namespace cgmath
{
  /// General m x n -matrix.
  template<typename scalar, unsigned m, unsigned n>
  class matrix_mn
  {
  public:
  
    /// Constructor for uninitialized matrix. 
    matrix_mn ()
    {}

    /// Constructor with initialization.
    explicit matrix_mn (const scalar& a)
    {
      std::fill (&mat[0][0], &mat[0][0] + m*n, a);
    }

    /// Copy constructor.
    matrix_mn (const matrix_mn& rhs)
    {
      std::copy (&rhs.mat[0][0], &rhs.mat[0][0] + m*n, &mat[0][0]); 
    }

    /// Conversion to pointer.
    operator scalar* () 
    {
      return &mat[0][0];
    }

    /// Conversion to const pointer.
    operator const scalar* () const
    {
      return &mat[0][0];
    }

    /// Const element access. 
    const scalar& operator () (unsigned i, unsigned j) const
    {
      return mat[j][i];
    }

    /// Element access. 
    scalar& operator () (unsigned i, unsigned j) 
    {
      return mat[j][i];
    }

    /// Identity matrix. 
    static matrix_mn identity (void)
    {
      return eye();
    }

    /// Identity matrix.
    static matrix_mn eye (void)
    {
      matrix_mn result (static_cast<scalar>(0));
      for (unsigned i = 0; i < std::min(m,n); ++i)
	result(i,i) = static_cast<scalar>(1);
      return result;
    }

    /// Uniform [0,1] random matrix.
    static matrix_mn rand ()
    {
      matrix_mn result (static_cast<scalar>(0));
      for (unsigned i = 0; i < m; ++i)
	for (unsigned j = 0; j < n; ++j)
	  result(i,j) = static_cast<scalar>(std::rand()/static_cast<double>(RAND_MAX));
      return result;
    }

    /// Conversion to another scalar type.
    template<typename scalar2>
    operator matrix_mn< scalar2, m, n > () const
    {
      matrix_mn< scalar2, m, n > result;
      for (unsigned i = 0; i < m; ++i)
	for (unsigned j = 0; j < n; ++j)
	  result(i,j) = static_cast<scalar2>((*this)(i,j));
      return result;
    }
   
    /// Extract row i as vector.
    vector_n<scalar, n> row (unsigned i) const
    {
      vector_n<scalar, n> r;
      for (unsigned j = 0; j < n; ++j)
	r(j) = mat[j][i];
      return r;
    }

    /// Extract col i as vector.
    vector_n<scalar, m> col (unsigned i) const
    {
      vector_n<scalar, n> r;
      for (unsigned j = 0; j < m; ++j)
	r(j) = mat[i][j];
      return r;
    }

    /// In-place multiply by scalar.
    matrix_mn<scalar,m,n>& operator*= (const scalar& a)
    {
      matrix_mn<scalar,m,n> AB;
      for (unsigned i = 0; i < m; ++i)
	for (unsigned j = 0; j < n; ++j)
	  (*this)(i,j) *= a;
      return *this;
    }
    
    /// In-place accumulation.
    matrix_mn<scalar,m,n>& operator+= (const matrix_mn<scalar, m, n>& A)
    {
      matrix_mn<scalar,m,n> AB;
      for (unsigned i = 0; i < m; ++i)
	for (unsigned j = 0; j < n; ++j)
	  (*this)(i,j) += A(i,j);
      return *this;
    }

    /// In-place subtraction.
    matrix_mn<scalar,m,n>& operator-= (const matrix_mn<scalar, m, n>& A)
    {
      matrix_mn<scalar,m,n> AB;
      for (unsigned i = 0; i < m; ++i)
	for (unsigned j = 0; j < n; ++j)
	  (*this)(i,j) -= A(i,j);
      return *this;
    }

  protected:
    scalar mat[n][m];
  };

  /// Outputting matrices.
  template<typename scalar, unsigned m, unsigned n>
  std::ostream& operator<< (std::ostream& os, 
			    const matrix_mn<scalar, m, n>& A)
  {
    for (unsigned i = 0; i < m; ++i)
      {
	for (unsigned j = 0; j < n; ++j)
	  os << A(i,j) << " ";
	os << std::endl;
      }
    return os;
  }
  
  /// Scalar-matrix multiply.
  template<typename scalar, unsigned m, unsigned n>
  matrix_mn<scalar,m,n> operator* (const scalar& a,
				   const matrix_mn<scalar, m, n>& A)
  {
    matrix_mn<scalar,m,n> aA;
    for (unsigned i = 0; i < m; ++i)
      for (unsigned j = 0; j < n; ++j)
	aA(i,j) = a * A(i,j);
    return aA;
  }

  
  /// Matrix-matrix multiply.
  template<typename scalar, unsigned m, unsigned k, unsigned n>
  matrix_mn<scalar,m,n> operator* (const matrix_mn<scalar, m, k>& A,
				   const matrix_mn<scalar, k, n>& B)
  {
    matrix_mn<scalar,m,n> AB;
    for (unsigned i = 0; i < m; ++i)
      for (unsigned j = 0; j < n; ++j)
	{
	  scalar ABij = static_cast<scalar>(0);
	  for (unsigned l = 0; l < k; ++l)
	    ABij += A(i,l) * B(l,j);
	  AB(i,j) = ABij;
	}
    return AB;
  }

  /// Matrix-matrix addition.
  template<typename scalar, unsigned m, unsigned n>
  matrix_mn<scalar,m,n> operator+ (const matrix_mn<scalar, m, n>& A,
				   const matrix_mn<scalar, m, n>& B)
  {
    matrix_mn<scalar,m,n> ApB;
    for (unsigned i = 0; i < m; ++i)
      for (unsigned j = 0; j < n; ++j)
	ApB(i,j) = A(i,j) + B(i,j);
    return ApB;
  }

  /// Matrix negation.
  template<typename scalar, unsigned m, unsigned n>
  matrix_mn<scalar,m,n> operator- (const matrix_mn<scalar, m, n>& A)
  {
    matrix_mn<scalar,m,n> mA;
    for (unsigned i = 0; i < m; ++i)
      for (unsigned j = 0; j < n; ++j)
	mA(i,j) = -A(i,j);
    return mA;
  }

  /// Matrix subtraction.
  template<typename scalar, unsigned m, unsigned n>
  matrix_mn<scalar,m,n> operator- (const matrix_mn<scalar, m, n>& A,
				   const matrix_mn<scalar, m, n>& B)
  {
    matrix_mn<scalar,m,n> AmB;
    for (unsigned i = 0; i < m; ++i)
      for (unsigned j = 0; j < n; ++j)
	AmB(i,j) = A(i,j) - B(i,j);
    return AmB;
  }
  

  /// Matrix-column vector multiplication.
  template<typename scalar, unsigned m, unsigned n>
  vector_n<scalar,m> operator* (const matrix_mn<scalar, m, n>& A,
				const vector_n<scalar, n>&     x)
  {
    vector_n<scalar,m> Ax;
    for (unsigned i = 0; i < m; ++i)
      {
	scalar Axi = static_cast<scalar>(0);
	for (unsigned j = 0; j < n; ++j)
	  Axi += A(i,j) * x(j);
	Ax(i) = Axi;
      }
    return Ax;
  }

  /// Row vector-Matrix multiplication.
  template<typename scalar, unsigned m, unsigned n>
  vector_n<scalar, m> operator* (const vector_n<scalar, m>&     x,
				 const matrix_mn<scalar, m, n>& A)
  {
    vector_n<scalar,n> xA;
    for (unsigned j = 0; j < n; ++j)
      {
	scalar xAj = static_cast<scalar>(0);
	for (unsigned i = 0; i < m; ++i)
	  xAj += x(i) * A(i,j);
	xA(j) = xAj;
      }
    return xA;
  }

  /// Vector-vector outer product.
  template<typename scalar, unsigned m, unsigned n>
  matrix_mn<scalar, m, n> outer (const vector_n<scalar,m>& u,
				 const vector_n<scalar,n>& v)
  {
    matrix_mn<scalar, m, n> uv;
    for (unsigned i = 0; i < m; ++i)
      for (unsigned j = 0; j < n; ++j)
	uv(i,j) = u(i) * v(j);
    return uv;
  }

  /// LU-decomposition.
  template<typename scalar, unsigned m, unsigned n>
  bool lu (matrix_mn<scalar,m,n>& A, unsigned P[m])
  {
    for (unsigned i = 0; i < m; ++i)
      P[i] = i;
    for (unsigned i = 0; i < n; ++i)
      {
	// Pivoting
	unsigned max_row = i;
	for (unsigned j = i + 1; j < m; ++j)
	  if (fabs(A(j, i)) > fabs(A(max_row, i)))
	    max_row = j;
	if (fabs(A(max_row, i)) < std::numeric_limits<scalar>::epsilon())
	  return false;
	// Swap rows
	if (max_row != i)
	  {
	    std::swap (P[max_row], P[i]);
	    for (unsigned k = 0; k < n; ++k)
	      std::swap (A(i, k), A(max_row, k));
	  }
	scalar Aii = A(i, i);
	for (unsigned j = i + 1; j < m; ++j)
	  {
	    A(j, i) /= Aii;
	    for (unsigned k = i+1; k < n; ++k)
	      A(j, k) -= A(j, i) * A(i,k);
	  }
      }
    return true;
  }

  /// Linear equation solver.
  template<typename scalar, unsigned m, unsigned n>
  vector_n<scalar,n> lu_solve (matrix_mn<scalar,m,n>& LU, 
			       unsigned P[m], 
			       const vector_n<scalar,n>& b)
  {
    vector_n<scalar,n> x(0);
    for (unsigned i = 0; i < m; ++i)
      {
	scalar s = static_cast<scalar>(0);
	for (unsigned j = 0; j < i; ++j)
	  s += LU(i,j) * x(j);
	x(i) = b(P[i]) - s;
      }
    for (int i = m-1; i >= 0; --i)
      {
	scalar s = static_cast<scalar>(0);
	for (unsigned j = i+1; j < m; ++j)
	  s += LU(i,j) * x(j);
	x(i) = (x(i) - s)/LU(i,i);
      }
    return x;
  }

  /// Linear equation solver for multiple right hand sides.
  template<typename scalar, unsigned m, unsigned n, unsigned k>
  matrix_mn<scalar,n,k> lu_solve (matrix_mn<scalar,m,n>& LU, 
				 unsigned P[m], 
				 const matrix_mn<scalar,n,k>& B)
  {
    matrix_mn<scalar,n,k> X(0);
    for (unsigned l = 0; l < k; ++l)
      for (unsigned i = 0; i < m; ++i)
	{
	  scalar s = static_cast<scalar>(0);
	  for (unsigned j = 0; j < i; ++j)
	    s += LU(i,j) * X(j,l);
	  X(i,l) = B(P[i],l) - s;
	}
    for (unsigned l = 0; l < k; ++l)
      for (int i = m-1; i >= 0; --i)
	{
	  scalar s = static_cast<scalar>(0);
	  for (unsigned j = i+1; j < m; ++j)
	    s += LU(i,j) * X(j,l);
	  X(i,l) = (X(i,l) - s)/LU(i,i);
	}
    return X;
  }

  /// Matrix inverse.
  template<typename scalar, unsigned m>
  matrix_mn<scalar,m,m> inv (const matrix_mn<scalar,m,m>& A)
  {
    matrix_mn<scalar,m,m> LU = A;
    unsigned P[m];
    bool nonsingular = lu (LU, P);
    if (!nonsingular)
      throw std::runtime_error ("Tried to invert singular matrix.");
    return lu_solve (LU, P, matrix_mn<scalar,m,m>::eye());
  }

  /// Frobenius norm.
  template<typename scalar, unsigned m>
  double norm (const matrix_mn<scalar,m,m>& A)
  {
    scalar norm = 0;
    for (unsigned i = 0; i < m; ++i)
      for (unsigned j = 0; j < m; ++j)
	norm += A(i,j) * A(i, j);
    return sqrt(norm);
  }

  /// Max norm.
  template<typename scalar, unsigned m>
  scalar norm_max (const matrix_mn<scalar,m,m>& A)
  {
    scalar norm = fabs(A(0,0));
    for (unsigned i = 0; i < m; ++i)
      for (unsigned j = 0; j < m; ++j)
	norm = max(norm, fabs(A(i,j)));
    return norm;
  }

  /// Matrix transposition.
  template<typename scalar, unsigned m, unsigned n>
  matrix_mn<scalar, n, m> transpose (const matrix_mn<scalar,m,n>& A)
  {
    matrix_mn<scalar, n, m> At;
    for (unsigned i = 0; i < m; ++i)
      for (unsigned j = 0; j < n; ++j)
	At(j,i) = A(i,j);
    return At;
  }
  
  /// Matrix determinant.
  template<typename scalar, unsigned m>
  scalar det (const matrix_mn<scalar,m,m>& _A)
  {
    bool negate = false;
    matrix_mn<scalar,m,m> A = _A;
    unsigned P[m];
    for (unsigned i = 0; i < m; ++i)
      P[i] = i;
    for (unsigned i = 0; i < m; ++i)
      {
	// Pivoting
	unsigned max_row = i;
	for (unsigned j = i + 1; j < m; ++j)
	  if (fabs(A(j, i)) > fabs(A(max_row, i)))
	    max_row = j;
	if (fabs(A(max_row, i)) < std::numeric_limits<scalar>::epsilon())
	  return static_cast<scalar>(0);
	// Swap rows
	if (max_row != i)
	  {
	    std::swap (P[max_row], P[i]);
	    for (unsigned k = i; k < m; ++k)
	      std::swap (A(i, k), A(max_row, k));
	    negate = !negate;
	  }
	scalar Aii = A(i, i);
	for (unsigned j = i + 1; j < m; ++j)
	  {
	    A(j, i) /= Aii;
	    for (unsigned k = i+1; k < m; ++k)
	      A(j, k) -= A(j, i) * A(i,k);
	  }
      }
    scalar det = 1.0;
    for (unsigned i = 0; i < m; ++i)
      det *= A(i,i);
    return negate ? -det : det;
  }
  
  /// Givens rotation from the right in-place. 
  template<typename scalar, unsigned m, unsigned n>
  inline void apply_right_givens (matrix_mn<scalar, m, n>& A, 
				  unsigned i,
				  unsigned j,
				  double c,
				  double s)
  {
    for (unsigned k = 0; k < m; ++k)
    {
      double A_ki =  c * A(k,i) + s * A(k,j);
      double A_kj = -s * A(k,i) + c * A(k,j);
      A(k,i) = A_ki;
      A(k,j) = A_kj;
    }
  }

  /// Givens rotation from the left in-place. 
  template<typename scalar, unsigned m, unsigned n>
  inline void apply_left_givens (matrix_mn<scalar, m, n>& A, 
				 unsigned i,
				 unsigned j,
				 double c,
				 double s)
  {
    for (unsigned k = 0; k < n; ++k)
      {
	double A_ik =  c * A(i,k) - s * A(j,k);
	double A_jk =  s * A(i,k) + c * A(j,k);
	A(i,k) = A_ik;
	A(j,k) = A_jk;
      }
  }
  
  /// Symmetric schur triangulation (see Golub & van Loan)
  inline bool sym_schur2 (double App, 
			  double Apq,
			  double Aqq, 
			  double* c, 
			  double* s)
  {
    int rotate;
    if (fabs(Apq) > 1e-12)
      {
	double tau = (Aqq - App)/(2.0 * Apq);
	double t;
	if (tau >= 0.0)
	  t =  1.0/ (tau + sqrt(1 + tau*tau));
	else
	  t = -1.0/(-tau + sqrt(1 + tau*tau));
	*c = 1/sqrt(1 + t*t);
	*s = t * *c;
	rotate = true;
      }
    else
      {
	*c = 1.0;
	*s = 0.0;
	rotate = false;
      }
    return rotate;
  }
  
  /// Sum of squared off-diagonal entries.
  template<typename scalar, unsigned m>
  scalar sym_off (const matrix_mn<scalar, m, m>& A)
  {
    scalar off = 0;
    for (unsigned i = 0; i < m; ++i)
      for (unsigned j = i+1; j < m; ++j)
	off += A(i,j) * A(i, j);
    return 2 * off;
  }

  /// Given symmetric A computes spectral (or eigen-) decomposition 
  /// A = VDV^T, where A is overwritten with D and off(A) < tol*||A||
  /// See Golub & Van Loan sect. 8.4.4.
  template<typename scalar, unsigned m>
  void eig (matrix_mn<scalar,m,m>& A, 
	    matrix_mn<scalar,m,m>& V, 
	    double tol = 1e-12)
  {
    V = matrix_mn<scalar,m,m>::identity ();
    double eps = tol * norm(A);
    while (sym_off(A) > eps)
      {
	for (unsigned p = 0; p < m; ++p)
	  for (unsigned q = p+1; q < m; ++q)
	    {
	      double c, s;
	      if (sym_schur2 (A(p,p), A(p,q), A(q,q), &c, &s))
		{
		  apply_left_givens  (A, p, q, c,  s);
		  apply_right_givens (A, p, q, c, -s);
		  apply_right_givens (V, p, q, c, -s);
		}
	    }
      }
  }
}

#endif
