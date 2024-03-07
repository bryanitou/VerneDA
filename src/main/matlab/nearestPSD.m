
function Ahat = nearestPSD(A,method)
% Ahat = nearestPSD(A,method)
% nearestPSD - The nearest (in Frobenius norm) symmetric Positive Semi-Definite matrix to A
% Matrix A may be real or complex
%
% From Higham: "The nearest symmetric positive semidefinite matrix in the
% Frobenius norm to an arbitrary real matrix A is shown to be (B + H)/2,
% where H is the symmetric polar factor of B = (A + A')/2."
%
% See for proof of method SVD
% Higham NJ. Computing a nearest symmetric positive semidefinite matrix. 
% Linear algebra and its applications. 1988 May 1;103:103-18.
%  (http://www.sciencedirect.com/science/article/pii/0024379588902236)
%
% arguments: (input)
%  A - square matrix, which will be converted to the nearest Symmetric
%    Positive Definite Matrix.
%
%  method - 'svd' or eig', [Optional, default= 'svd']
%             'svd' is the method of Higham using the symmetric polar factor.
%             'eig' rectifies the eigvalues and recomposes the matrix.
%             While theorically equivalent, method 'svd' is more numerically stable
%             especially in cases of high co-linearity, and tends to returns an
%             Ahat slightly closer to A than method 'eig'. Therefore, while method 'eig' executes
%             faster, it is not recomended.
%
% Arguments: (output)
%  Ahat - The matrix chosen as the nearest PSD matrix to A.
%
%
% This function was modified from nearestSPD.m by John D'Errico 
%   (https://www.mathworks.com/matlabcentral/fileexchange/42885-nearestspd)
%
%  Burke Rosen added support for complex inputs, added the 'eig' method and made other
%  small changes. Written in R2019b but should be broadly backwards compatible.
%
%
% Copyright (c) 2019, Burke Rosen
% Copyright (c) 2013, John D'Errico
% All rights reserved.
% 
% Redistribution and use in source and binary forms, with or without
% modification, are permitted provided that the following conditions are met:
% 
% * Redistributions of source code must retain the above copyright notice, this
% list of conditions and the following disclaimer.
% 
% * Redistributions in binary form must reproduce the above copyright notice,
% this list of conditions and the following disclaimer in the documentation
% and/or other materials provided with the distribution
% * Neither the name of nor the names of its
% contributors may be used to endorse or promote products derived from this
% software without specific prior written permission.
% THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
% AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
% IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
% DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
% FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
% DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
% SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
% CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
% OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
% OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.



switch nargin
  case 0
    help nearestSPD;
    return
  case 1
    method = 'svd';
  case 2
    if ~ismember(method,{'eig','svd'})
      error('Valid methods are ''eig'' or ''svd''');
    end
  otherwise
    error('Too many arguments.')
end

A = double(A);
% check if A is PSD (and square)
[~,p] = chol(A);
if p == 0
  Ahat = A;
  warning('A is already PSD, doing nothing.')
  return;
end

n = size(A,1);
iscmplx = ~isreal(A);


% symmetrize A into B
if iscmplx % make Hermitian instead of symmetric
  hermSign=tril(-ones(n),-1)+triu(ones(n),1);
  B = complex((real(A) + real(A).')./2,...
              (imag(A) + imag(A).')./2.*hermSign);
else
  B = (A + A.')/2;
end

switch method
  case 'svd'
    % Compute the symmetric polar factor of B. Call it H.
    % Clearly H is itself PSD.
    [~,Sigma,V] = svd(B);
    H = V*Sigma*V';
    % get Ahat in the above formula
    Ahat = (B+H)/2;
  case 'eig'
    % rectify eigenvalues
    [V,D] = eig(A);
    D = max(D,0);
    Ahat = V*D*V';
end

% ensure Ahat symmetry 
if iscmplx  % make Hermitian instead of symmetric
  Ahat = complex((real(Ahat) + real(Ahat).')./2,...
            (abs(imag(Ahat)) + abs(imag(Ahat)).')./2.*hermSign);
else
  Ahat = (Ahat + Ahat.')/2;
end

% test that Ahat is in fact PSD. if it is not so, then tweak it just a bit.
p = 1;
k = 0;
while p ~= 0
  [~,p] = chol(Ahat);
  k = k + 1;
  if p ~= 0
    % Ahat failed the chol test. It must have been just a hair off,
    % due to floating point trash, so it is simplest now just to
    % tweak by adding a tiny multiple of an identity matrix.
    mineig = min(eig(Ahat));
    Ahat = Ahat + (-mineig*k.^2 + eps(mineig))*eye(size(A));
  end
end
