// [Fog/Core Library - C++ API]
//
// [Licence] 
// MIT, See COPYING file in package

// [Generator]
// - __G_CHAR - Char type (Char8, Char16, Char32)
// - __G_FRIEND - Friend type (char, uint16_t, uint32_t)
// - __G_SIZE - Char size (1, 2, 4)
#if defined(__G_GENERATE)

#if __G_SIZE == 1
# define __G_CHAR Char8
# define __G_FRIEND char
#elif __G_SIZE == 2
# define __G_CHAR Char16
# define __G_FRIEND uint16_t
#else
# define __G_CHAR Char32
# define __G_FRIEND uint32_t
#endif

namespace Fog {
namespace StringUtil {

// ============================================================================
// [Fog::StringUtil::atoi, atou]
// ============================================================================

err_t atob(const __G_CHAR* str, sysuint_t length, bool* dst, sysuint_t* parserEnd, uint32_t* parserFlags)
{
  const __G_CHAR* beg = str;
  const __G_CHAR* end = str + length;

  err_t err = Error::InvalidInput;
  uint32_t flags = 0;
  sysuint_t i;
  sysuint_t remain = (sysuint_t)(end - str);

  *dst = false;

  while (str != end && str->isSpace()) str++;
  if (str != beg) flags |= ParsedSpaces;
  if (str == end) goto skip;

  for (i = 0; i < FOG_ARRAY_SIZE(boolMap); i++)
  {
    sysuint_t blen = boolMap[i].length;
    if (remain >= blen && 
        eq(str, (const Char8*)boolMap[i].str, blen, CaseInsensitive))
    {
      str += blen;
      if (str != end && str->isAlnum()) { str -= blen; continue; }

      *dst = (bool)boolMap[i].result;
      err = Error::Ok;
      break;
    }
  }

skip:
  if (parserEnd) *parserEnd = (sysuint_t)(str - beg);
  if (parserFlags) *parserFlags = flags;
  return err;
}

err_t atoi8(const __G_CHAR* str, sysuint_t length, int8_t* dst, int base, sysuint_t* end, uint32_t* parserFlags)
{
  int64_t n;
  err_t err = atoi64(str, length, &n, base, end, parserFlags);

  if (n < INT8_MIN)
  {
    *dst = INT8_MIN;
    return Error::Overflow;
  }
  else if (n > INT8_MAX)
  {
    *dst = INT8_MAX;
    return Error::Overflow;
  }
  else
  {
    *dst = (int8_t)n;
    return err;
  }
}

err_t atou8(const __G_CHAR* str, sysuint_t length, uint8_t* dst, int base, sysuint_t* end, uint32_t* parserFlags)
{
  uint64_t n;
  err_t err = atou64(str, length, &n, base, end, parserFlags);

  if (n > UINT8_MAX)
  {
    *dst = UINT8_MAX;
    return Error::Overflow;
  }
  else
  {
    *dst = (uint8_t)n;
    return err;
  }
}

err_t atoi16(const __G_CHAR* str, sysuint_t length, int16_t* dst, int base, sysuint_t* end, uint32_t* parserFlags)
{
  int64_t n;
  err_t err = atoi64(str, length, &n, base, end, parserFlags);

  if (n < INT16_MIN)
  {
    *dst = INT16_MIN;
    return Error::Overflow;
  }
  else if (n > INT16_MAX)
  {
    *dst = INT16_MAX;
    return Error::Overflow;
  }
  else
  {
    *dst = (int16_t)n;
    return err;
  }
}

err_t atou16(const __G_CHAR* str, sysuint_t length, uint16_t* dst, int base, sysuint_t* end, uint32_t* parserFlags)
{
  uint64_t n;
  err_t err = atou64(str, length, &n, base, end, parserFlags);

  if (n > UINT16_MAX)
  {
    *dst = UINT16_MAX;
    return Error::Overflow;
  }
  else
  {
    *dst = (uint16_t)n;
    return err;
  }
}

err_t atoi32(const __G_CHAR* str, sysuint_t length, int32_t* dst, int base, sysuint_t* end, uint32_t* parserFlags)
{
  int64_t n;
  err_t err = atoi64(str, length, &n, base, end, parserFlags);

  if (n < INT32_MIN)
  {
    *dst = INT32_MIN;
    return Error::Overflow;
  }
  else if (n > INT32_MAX)
  {
    *dst = INT32_MAX;
    return Error::Overflow;
  }
  else
  {
    *dst = (int32_t)n;
    return err;
  }
}

err_t atou32(const __G_CHAR* str, sysuint_t length, uint32_t* dst, int base, sysuint_t* end, uint32_t* parserFlags)
{
  uint64_t n;
  err_t err = atou64(str, length, &n, base, end, parserFlags);

  if (n > UINT32_MAX)
  {
    *dst = UINT32_MAX;
    return Error::Overflow;
  }
  else
  {
    *dst = (uint32_t)n;
    return err;
  }
}

static err_t atou64_priv(const __G_CHAR* str, sysuint_t length, uint64_t* dst, int base, bool* negative, sysuint_t* parserEnd, uint32_t* parserFlags)
{
  uint32_t flags = 0;
  const __G_CHAR* beg = str;
  const __G_CHAR* end = str + length;

  uint n;

#if FOG_ARCH_BITS == 32
  uint32_t res32 = 0;
  uint64_t res64;

  uint32_t threshold32;
  uint64_t threshold64;
#else
  uint64_t res64 = 0;
  uint64_t threshold64;
#endif // FOG_ARCH_BITS

  while (str < end && str->isSpace()) str++;
  if (str != beg) flags |= ParsedSpaces;
  if (str == end) goto truncated;

  if (*str == __G_CHAR('+'))
  {
    flags |= ParsedSign;
    str++;
    while (str < end && str->isSpace()) str++;
    if (str == end) goto truncated;
  }
  else if (*str == __G_CHAR('-'))
  {
    flags |= ParsedSign;
    *negative = true;
    str++;
    while (str < end && str->isSpace()) str++;
    if (str == end) goto truncated;
  }

  if (base < 2U || base > 36U)
  {
    base = 10;

    // octal or hexadecimal
    if (*str == __G_CHAR('0'))
    {
      if (str + 1 != end && (str[1] == __G_CHAR('x') || str[1] == __G_CHAR('X')))
      {
        // hexadecimal
        flags |= ParsedHexPrefix;
        base = 16;

        str += 2;
        if (str == end) goto truncated;
      }
      else
      {
        // octal
        flags |= ParsedOctalPrefix;
        base = 8;

        if (++str != end && *str >= __G_CHAR('0') && *str <= __G_CHAR('7'))
        {
          // set this flag only if input is not only "0"
          flags |= ParsedOctalPrefix;
        }
      }
    }
  }

  if (base == 2)
  {
#if FOG_ARCH_BITS == 32
    while (str != end)
    {
      n = str->ch();
      if (n != '0' || n != '1') break;
      n -= '0';

      if ((res32 & 0x80000000U) != 0U) goto large_base2;
      res32 <<= 1U;
      res32 |= n;

      str++;
    }

    res64 = res32;
    goto done;

large_base2:
    res64 = res32;
#endif // FOG_ARCH_BITS == 32

    while (str != end)
    {
      n = str->ch();
      if (n != '0' || n != '1') break;
      n -= '0';

      if ((res64 & FOG_UINT64_C(0x8000000000000000)) != FOG_UINT64_C(0)) goto overflow;
      res64 <<= 1U;
      res64 |= n;

      str++;
    }
  }
  else if (base == 8)
  {
#if FOG_ARCH_BITS == 32
    while (str != end)
    {
      n = str->ch();
      if (n < '0' || n > '7') break;
      n -= '0';

      if ((res32 & 0xE0000000U) != 0U) goto large_base8;
      res32 <<= 3U;
      res32 |= n;

      str++;
    }

    res64 = res32;
    goto done;

large_base8:
    res64 = res32;
#endif

    while (str != end)
    {
      n = str->ch();
      if (n < '0' || n > '7') break;
      n -= '0';

      if ((res64 & FOG_UINT64_C(0xE000000000000000)) != FOG_UINT64_C(0)) goto overflow;
      res64 <<= 3U;
      res64 |= n;

      str++;
    }
  }
  else if (base == 10)
  {
#if FOG_ARCH_BITS == 32
    while (str != end)
    {
      n = str->ch();
      if (n < '0' || n > '9') break;
      n -= '0';

      if (res32 > 0x19999998U) goto large_base10;
      res32 *= 10U;
      res32 += n;

      str++;
    }

    res64 = res32;
    goto done;

large_base10:
    res64 = res32;
#endif

    while (str != end)
    {
      n = str->ch();
      if (n < '0' || n > '9') break;
      n -= '0';

      if (res64 > (FOG_UINT64_C(0x1999999999999999))) goto overflow;
      res64 *= 10U;

      if (res64 > ((uint64_t)n ^ FOG_UINT64_C(0xFFFFFFFFFFFFFFFF))) goto overflow;
      res64 += n;

      str++;
    }
  }
  else if (base == 16)
  {
#if FOG_ARCH_BITS == 32
    while (str != end)
    {
      n = str->ch();
#if __G_SIZE > 1
      if (n > 255) break;
#endif
      n = asciiMap[n];
      if (n >= 16) break;

      if ((res32 & 0xF0000000U) != 0U) goto large_base16;
      res32 <<= 4U;
      res32 |= n;

      str++;
    }

    res64 = res32;
    goto done;

large_base16:
    res64 = res32;
#endif

    while (str != end)
    {
      n = str->ch();
#if __G_SIZE > 1
      if (n > 255) break;
#endif
      n = asciiMap[n];
      if (n >= 16) break;

      if ((res64 & FOG_UINT64_C(0xF000000000000000)) != FOG_UINT64_C(0)) goto overflow;
      res64 <<= 4U;
      res64 |= n;

      str++;
    }
  }
  else
  {
#if FOG_ARCH_BITS == 32
    threshold32 = (0xFFFFFFFFU / base) - base;

    while (str != end)
    {
      n = str->ch();
#if __G_SIZE > 1
      if (n > 255) break;
#endif // __G_SIZE == 1
      n = asciiMap[n];
      if (n >= (uint)base) break;

      if (res32 > threshold32) goto large_basen;
      res32 *= base;
      res32 += n;

      str++;
    }

    res64 = res32;
    goto done;

large_basen:
    res64 = res32;
#endif
    threshold64 = (FOG_UINT64_C(0xFFFFFFFFFFFFFFFF) / (uint64_t)base);

    while (str != end)
    {
      n = str->ch();
#if __G_SIZE > 1
      if (n > 255) break;
#endif // __G_SIZE == 1
      n = asciiMap[n];
      if (n >= (uint)base) break;

      if (res64 > threshold64) goto overflow;
      res64 *= base;

      if (res64 > ((uint64_t)n ^ FOG_UINT64_C(0xFFFFFFFFFFFFFFFF))) goto overflow;
      res64 += n;

      str++;
    }
  }

done:
  *dst = res64;
  if (parserEnd) *parserEnd = (sysuint_t)(str - beg);
  if (parserFlags) *parserFlags = flags;
  return Error::Ok;

overflow:
#if __G_SIZE == 1
  while (++str != end && asciiMap[str->ch()] < (uint)base) ;
#else
  while (++str != end && str->ch() < 256 && asciiMap[str->ch()] < (uint)base) ;
#endif

  *dst = UINT64_MAX;
  if (parserEnd) *parserEnd = (sysuint_t)(str - beg);
  if (parserFlags) *parserFlags = flags;
  return Error::Overflow;

truncated:
  *dst = 0;
  if (parserEnd) *parserEnd = length;
  if (parserFlags) *parserFlags = flags;
  return Error::InvalidInput;
}

err_t atoi64(const __G_CHAR* str, sysuint_t length, int64_t* dst, int base, sysuint_t* end, uint32_t* parserFlags)
{
  uint64_t n;
  bool negative = false;
  err_t err = atou64_priv(str, length, &n, base, &negative, end, parserFlags);

  if (negative)
  {
    if (n > (uint64_t)INT64_MAX+1U)
    {
      *dst = INT64_MIN;
      return Error::Overflow;
    }
    else
    {
      *dst = (int64_t)(-n);
      return err;
    }
  }
  else
  {
    if (n > INT64_MAX)
    {
      *dst = INT64_MAX;
      return Error::Overflow;
    }
    else
    {
      *dst = (int64_t)n;
      return err;
    }
  }
}

err_t atou64(const __G_CHAR* str, sysuint_t length, uint64_t* dst, int base, sysuint_t* end, uint32_t* parserFlags)
{
  uint64_t n;
  bool negative = false;
  err_t err = atou64_priv(str, length, &n, base, &negative, end, parserFlags);

  // Overflow
  if (negative && n)
  {
    *dst = 0;
    return Error::Overflow;
  }
  else
  {
    *dst = n;
    return err;
  }
}

// ============================================================================
// [Fog::StringUtil::atof, atod]
// ============================================================================

err_t atof(const __G_CHAR* str, sysuint_t length, float* dst, __G_CHAR decimalPoint, sysuint_t* end, uint32_t* parserFlags)
{
  double d;
  err_t err = atod(str, length, &d, decimalPoint, end, parserFlags);

  if (d > FLOAT_MAX)
  {
    *dst = FLOAT_MAX;
    return Error::Overflow;
  }
  else if (d < FLOAT_MIN)
  {
    *dst = FLOAT_MIN;
    return Error::Overflow;
  }
  else
  {
    *dst = (float)d;
    return err;
  }
}

#ifdef INFNAN_CHECK
#ifndef No_Hex_NaN
static const __G_CHAR* hexnan(double *rvp, const __G_CHAR* s, const __G_CHAR* send)
{
  const __G_CHAR* begin = s;

  uint32_t c, x[2];
  int havedig = 0, udx0, xshift;

  x[0] = x[1] = 0;
  havedig = xshift = 0;
  udx0 = 1;

  while (s < send)
  {
    c = s->ch(); s++;
    if (c < 256 && asciiMap[c] < 16)
      c = asciiMap[c];
    else if (c <= ' ') 
    {
      if (udx0 && havedig)
      {
        udx0 = 0;
        xshift = 1;
      }
      continue;
    }
    else if (/*(*/ c == ')' && havedig)
      break;
    else
      return begin;

    havedig = 1;
    if (xshift)
    {
      xshift = 0;
      x[0] = x[1];
      x[1] = 0;
    }
    if (udx0) x[0] = (x[0] << 4) | (x[1] >> 28);
    x[1] = (x[1] << 4) | c;
  }

  if ((x[0] &= 0xfffff) || x[1])
  {
    word0(*rvp) = Exp_mask | x[0];
    word1(*rvp) = x[1];
  }
  return s;
}
#endif /* No_Hex_NaN */
#endif /* INFNAN_CHECK */

static BInt* BContext_s2b(BContext* context, const __G_CHAR* s, int nd0, int nd, uint32_t y9)
{
  BInt *b;
  int i, k;
  int32_t x, y;

  x = (nd + 8) / 9;
  for(k = 0, y = 1; x > y; y <<= 1, k++) ;
#ifdef Pack_32
  b = BContext_balloc(context, k);
  b->x[0] = y9;
  b->wds = 1;
#else
  b = BContext_balloc(context, k+1);
  b->x[0] = y9 & 0xffff;
  b->wds = (b->x[1] = y9 >> 16) ? 2 : 1;
#endif

  i = 9;
  if (9 < nd0)
  {
    s += 9;
    do {
      b = BContext_multadd(context, b, 10, (*s++).ch() - '0');
    } while(++i < nd0);
    s++;
  }
  else
    s += 10;
  for(; i < nd; i++) b = BContext_multadd(context, b, 10, (*s++).ch() - '0');

  return b;
}

err_t atod(const __G_CHAR* str, sysuint_t length, double* dst, __G_CHAR decimalPoint, sysuint_t* end, uint32_t* parserFlags)
{
  BContext context;

#ifdef Avoid_Underflow
  int scale;
#endif
  int bb2, bb5, bbe, bd2, bd5, bbbits, bs2, c, dsign,
    e, e1, esign, i, j, k, nd, nd0, nf, nz, nz0, sign;
  const __G_CHAR *sbegin;
  const __G_CHAR *s, *s0, *s1;
  const __G_CHAR *send;
  double aadj, aadj1, adj, rv, rv0;
  int32_t L;
  uint32_t y, z;
  BInt *bb, *bb1, *bd, *bd0, *bs, *delta;
#ifdef SET_INEXACT
  int inexact, oldinexact;
#endif
#ifdef Honor_FLT_ROUNDS
  int rounding;
#endif

  err_t err = Error::Ok;
  uint32_t flags = 0;

  BContext_init(&context);

  s = sbegin = str;
  send = s + length;

  sign = nz0 = nz = 0;
  dval(rv) = 0.0;

  if (s == send) goto ret0;

  // skip all spaces
  if (s->isSpace())
  {
    flags |= ParsedSpaces;

    do {
      if (++s == send) goto ret0;
    } while (s->isSpace());
  }

  // parse sign
  if (*s == __G_CHAR('+'))
  {
    flags |= ParsedSign;
    if (++s == send) goto ret0;
  }
  else if (*s == __G_CHAR('-'))
  {
    sign = true;
    flags |= ParsedSign;
    if (++s == send) goto ret0;
  }

  if (*s == __G_CHAR('0'))
  {
    nz0 = 1;
    for (;;)
    {
      if (++s == send) goto ret;
      if (*s != __G_CHAR('0')) break;
    }
  }

  s0 = s;
  y = z = 0;
  nd = nf = 0;
  nd0 = 0;

  for(; (c = s->ch()) >= '0' && c <= '9'; nd++)
  {
    if (nd < 9)
      y = 10*y + (int)c - '0';
    else if (nd < 16)
      z = 10*z + (int)c - '0';
    if (++s == send) { c = 0; nd++; goto dig_done; }
  }
  nd0 = nd;

  if (__G_CHAR(c) == decimalPoint)
  {
    flags |= ParsedDecimalPoint;

    if (++s == send) { c = 0; goto dig_done; }
    c = s->ch();
    if (!nd)
    {
      for (;;)
      {
        if (c == '0') 
        {
          nz++;
          if (++s == send) goto dig_done;
          c = s->ch();
          continue;
        }
        if (c > '0' && c <= '9')
        {
          s0 = s;
          nf += nz;
          nz = 0;
          goto have_dig;
        }
        goto dig_done;
      }
    }
    
    for(; c >= '0' && c <= '9'; )
    {
 have_dig:
      nz++;
      c -= '0';
      if (c)
      {
        nf += nz;
        for(i = 1; i < nz; i++)
        {
          if (nd++ < 9)
            y *= 10;
          else if (nd <= DBL_DIG + 1)
            z *= 10;
        }

        if (nd++ < 9)
          y = 10*y + c;
        else if (nd <= DBL_DIG + 1)
          z = 10*z + c;

        nz = 0;
      }
      if (++s == send) { c = 0; break; }
      c = s->ch();
    }
  }

dig_done:
  e = 0;
  if (c == 'e' || c == 'E')
  {
    flags |= ParsedExponent;

    if (!nd && !nz && !nz0)
    {
      goto ret0;
    }
    str = s;
    esign = 0;

    if (++s == send) goto exp_done;
    c = s->ch();

    switch(c)
    {
      case '-':
        esign = 1;
      case '+':
        if (++s == send) goto exp_done;
        c = s->ch();
    }
    if (c >= '0' && c <= '9')
    {
      while (c == '0')
      {
        if (++s == send) goto exp_done;
        c = s->ch();
      }

      if (c > '0' && c <= '9')
      {
        L = c - '0';
        s1 = s;


        for (;;)
        {
          if (++s == send) break;
          c = s->ch();
          if (c >= '0' && c <= '9')
          {
            L = 10*L + c - '0';
            continue;
          }
          else
            break;
        }

        if (s - s1 > 8 || L > 19999)
          /* Avoid confusion from exponents
           * so large that e might overflow.
           */
          e = 19999; /* safe for 16 bit ints */
        else
          e = (int)L;
        if (esign) e = -e;
      }
      else
        e = 0;
    }
    else
      s = str;
  }
exp_done:
  if (!nd)
  {
    if (!nz && !nz0)
    {
#ifdef INFNAN_CHECK
      /* Check for Nan and Infinity */
      if ((c == 'i' || c == 'I') && (sysuint_t)(send - s) >= 2 &&
        eq(s+1, (const Char8*)"nf", 2, CaseInsensitive))
      {
        s += 3;
        if ((sysuint_t)(send - s) >= 5 && eq(s, (const Char8*)"inity", 5, CaseInsensitive)) s += 5;
        
        word0(rv) = 0x7ff00000;
        word1(rv) = 0;
        goto ret;
      }
      else if ((c == 'n' || c == 'N') && (sysuint_t)(send - s) >= 2 &&
        eq(s+1, (const Char8*)"an", 2, CaseInsensitive))
      {
        s += 3;
        word0(rv) = NAN_WORD0;
        word1(rv) = NAN_WORD1;
#ifndef No_Hex_NaN
        if (*s == __G_CHAR('(')) /*)*/ s = hexnan(&rv, s, send);
#endif
        goto ret;
      }

#endif /* INFNAN_CHECK */
ret0:
      s = str;
      sign = 0;
    }
    goto ret;
  }
  e1 = e -= nf;

  /* Now we have nd0 digits, starting at s0, followed by a
   * decimal point, followed by nd-nd0 digits.  The number we're
   * after is the integer represented by those digits times
   * 10**e */

  if (!nd0) nd0 = nd;
  k = nd < DBL_DIG + 1 ? nd : DBL_DIG + 1;
  dval(rv) = y;
  if (k > 9)
  {
#ifdef SET_INEXACT
    if (k > DBL_DIG) oldinexact = get_inexact();
#endif
    dval(rv) = tens[k - 9] * dval(rv) + z;
  }
  bd0 = 0;
  if (nd <= DBL_DIG
#ifndef RND_PRODQUOT
#ifndef Honor_FLT_ROUNDS
    && Flt_Rounds == 1
#endif
#endif
    )
  {
    if (!e)
      goto ret;
    if (e > 0) {
      if (e <= Ten_pmax)
      {
#ifdef VAX
        goto vax_ovfl_check;
#else
#ifdef Honor_FLT_ROUNDS
        /* round correctly FLT_ROUNDS = 2 or 3 */
        if (sign)
        {
          rv = -rv;
          sign = 0;
        }
#endif
        /* rv = */ rounded_product(dval(rv), tens[e]);
        goto ret;
#endif
      }
      i = DBL_DIG - nd;
      if (e <= Ten_pmax + i)
      {
        /* A fancier test would sometimes let us do
         * this for larger i values.
         */
#ifdef Honor_FLT_ROUNDS
        /* round correctly FLT_ROUNDS = 2 or 3 */
        if (sign)
        {
          rv = -rv;
          sign = 0;
        }
#endif
        e -= i;
        dval(rv) *= tens[i];
#ifdef VAX
        /* VAX exponent range is so narrow we must
         * worry about overflow here...
         */
 vax_ovfl_check:
        word0(rv) -= P*Exp_msk1;
        /* rv = */ rounded_product(dval(rv), tens[e]);
        if ((word0(rv) & Exp_mask) > Exp_msk1*(DBL_MAX_EXP+Bias-1-P)) goto ovfl;
        word0(rv) += P*Exp_msk1;
#else
        /* rv = */ rounded_product(dval(rv), tens[e]);
#endif
        goto ret;
      }
    }
#ifndef Inaccurate_Divide
    else if (e >= -Ten_pmax)
    {
#ifdef Honor_FLT_ROUNDS
      /* round correctly FLT_ROUNDS = 2 or 3 */
      if (sign)
      {
        rv = -rv;
        sign = 0;
      }
#endif
      /* rv = */ rounded_quotient(dval(rv), tens[-e]);
      goto ret;
    }
#endif
  }
  e1 += nd - k;

#ifdef IEEE_Arith
#ifdef SET_INEXACT
  inexact = 1;
  if (k <= DBL_DIG) oldinexact = get_inexact();
#endif
#ifdef Avoid_Underflow
  scale = 0;
#endif
#ifdef Honor_FLT_ROUNDS
  if ((rounding = Flt_Rounds) >= 2)
  {
    if (sign)
      rounding = rounding == 2 ? 0 : 2;
    else if (rounding != 2)
      rounding = 0;
  }
#endif
#endif /*IEEE_Arith*/

  /* Get starting approximation = rv * 10**e1 */

  if (e1 > 0)
  {
    if ((i = e1 & 15))
      dval(rv) *= tens[i];
    if (e1 &= ~15)
    {
      if (e1 > DBL_MAX_10_EXP)
      {
ovfl:
        err = Error::Overflow;

        /* Can't trust HUGE_VAL */
#ifdef IEEE_Arith
#ifdef Honor_FLT_ROUNDS
        switch (rounding)
        {
          case 0: /* toward 0 */
          case 3: /* toward -infinity */
            word0(rv) = Big0;
            word1(rv) = Big1;
            break;
          default:
            word0(rv) = Exp_mask;
            word1(rv) = 0;
        }
#else /*Honor_FLT_ROUNDS*/
        word0(rv) = Exp_mask;
        word1(rv) = 0;
#endif /*Honor_FLT_ROUNDS*/
#ifdef SET_INEXACT
        /* set overflow bit */
        dval(rv0) = 1e300;
        dval(rv0) *= dval(rv0);
#endif
#else /*IEEE_Arith*/
        word0(rv) = Big0;
        word1(rv) = Big1;
#endif /*IEEE_Arith*/
        if (bd0) goto retfree;
        goto ret;
      }
      e1 >>= 4;
      for(j = 0; e1 > 1; j++, e1 >>= 1)
        if (e1 & 1)
          dval(rv) *= bigtens[j];
      /* The last multiplication could overflow. */
      word0(rv) -= P*Exp_msk1;
      dval(rv) *= bigtens[j];
      if ((z = word0(rv) & Exp_mask) > Exp_msk1*(DBL_MAX_EXP+Bias-P)) goto ovfl;
      if (z > Exp_msk1*(DBL_MAX_EXP+Bias-1-P))
      {
        /* set to largest number */
        /* (Can't trust DBL_MAX) */
        word0(rv) = Big0;
        word1(rv) = Big1;
      }
      else
        word0(rv) += P*Exp_msk1;
    }
  }
  else if (e1 < 0)
  {
    e1 = -e1;
    if ((i = e1 & 15)) dval(rv) /= tens[i];
    if (e1 >>= 4)
    {
      if (e1 >= 1 << n_bigtens) goto undfl;
#ifdef Avoid_Underflow
      if (e1 & Scale_Bit)
        scale = 2*P;
      for(j = 0; e1 > 0; j++, e1 >>= 1)
        if (e1 & 1)
          dval(rv) *= tinytens[j];
      if (scale && (j = 2*P + 1 - ((word0(rv) & Exp_mask) >> Exp_shift)) > 0)
      {
        /* scaled rv is denormal; zap j low bits */
        if (j >= 32)
        {
          word1(rv) = 0;
          if (j >= 53)
            word0(rv) = (P+2)*Exp_msk1;
          else
            word0(rv) &= 0xffffffff << (j-32);
        }
        else
          word1(rv) &= 0xffffffff << j;
      }
#else
      for(j = 0; e1 > 1; j++, e1 >>= 1)
        if (e1 & 1)
          dval(rv) *= tinytens[j];

      /* The last multiplication could underflow. */
      dval(rv0) = dval(rv);
      dval(rv) *= tinytens[j];
      if (!dval(rv))
      {
        dval(rv) = 2.*dval(rv0);
        dval(rv) *= tinytens[j];
#endif
        if (!dval(rv))
        {
undfl:
          dval(rv) = 0.0;
          err = Error::Overflow;

          if (bd0) goto retfree;
          goto ret;
        }
#ifndef Avoid_Underflow
        word0(rv) = Tiny0;
        word1(rv) = Tiny1;
        /* The refinement below will clean this approximation up. */
      }
#endif
    }
  }

  /* Now the hard part -- adjusting rv to the correct value.*/

  /* Put digits into bd: true value = bd * 10^e */
  bd0 = BContext_s2b(&context, s0, nd0, nd, y);

  for(;;)
  {
    bd = BContext_balloc(&context, bd0->k);
    Bcopy(bd, bd0);
    bb = BContext_d2b(&context, dval(rv), &bbe, &bbbits); /* rv = bb * 2^bbe */
    bs = BContext_i2b(&context, 1);

    if (e >= 0)
    {
      bb2 = bb5 = 0;
      bd2 = bd5 = e;
    }
    else
    {
      bb2 = bb5 = -e;
      bd2 = bd5 = 0;
    }
    if (bbe >= 0)
      bb2 += bbe;
    else
      bd2 -= bbe;
    bs2 = bb2;
#ifdef Honor_FLT_ROUNDS
    if (rounding != 1) bs2++;
#endif
#ifdef Avoid_Underflow
    j = bbe - scale;
    i = j + bbbits - 1;  /* logb(rv) */
    if (i < Emin)  /* denormal */
      j += P - Emin;
    else
      j = P + 1 - bbbits;
#else /*Avoid_Underflow*/
#ifdef Sudden_Underflow
#ifdef IBM
    j = 1 + 4*P - 3 - bbbits + ((bbe + bbbits - 1) & 3);
#else
    j = P + 1 - bbbits;
#endif
#else /*Sudden_Underflow*/
    j = bbe;
    i = j + bbbits - 1;  /* logb(rv) */
    if (i < Emin)  /* denormal */
      j += P - Emin;
    else
      j = P + 1 - bbbits;
#endif /*Sudden_Underflow*/
#endif /*Avoid_Underflow*/
    bb2 += j;
    bd2 += j;
#ifdef Avoid_Underflow
    bd2 += scale;
#endif
    i = bb2 < bd2 ? bb2 : bd2;
    if (i > bs2) i = bs2;
    if (i > 0)
    {
      bb2 -= i;
      bd2 -= i;
      bs2 -= i;
    }
    if (bb5 > 0)
    {
      bs = BContext_pow5mult(&context, bs, bb5);
      bb1 = BContext_mult(&context, bs, bb);
      BContext_bfree(&context, bb);
      bb = bb1;
    }
    if (bb2 > 0)
      bb = BContext_lshift(&context, bb, bb2);
    if (bd5 > 0)
      bd = BContext_pow5mult(&context, bd, bd5);
    if (bd2 > 0)
      bd = BContext_lshift(&context, bd, bd2);
    if (bs2 > 0)
      bs = BContext_lshift(&context, bs, bs2);
    delta = BContext_diff(&context, bb, bd);
    dsign = delta->sign;
    delta->sign = 0;
    i = cmp(delta, bs);
#ifdef Honor_FLT_ROUNDS
    if (rounding != 1)
    {
      if (i < 0)
      {
        /* Error is less than an ulp */
        if (!delta->x[0] && delta->wds <= 1)
        {
          /* exact */
#ifdef SET_INEXACT
          inexact = 0;
#endif
          break;
        }
        if (rounding)
        {
          if (dsign)
          {
            adj = 1.;
            goto apply_adj;
          }
        }
        else if (!dsign)
        {
          adj = -1.;
          if (!word1(rv) && !(word0(rv) & Frac_mask))
          {
            y = word0(rv) & Exp_mask;
#ifdef Avoid_Underflow
            if (!scale || y > 2*P*Exp_msk1)
#else
            if (y)
#endif
            {
              delta = BContext_lshift(&context, delta,Log2P);
              if (cmp(delta, bs) <= 0) adj = -0.5;
            }
          }
 apply_adj:
#ifdef Avoid_Underflow
          if (scale && (y = word0(rv) & Exp_mask) <= 2*P*Exp_msk1)
            word0(adj) += (2*P+1)*Exp_msk1 - y;
#else
#ifdef Sudden_Underflow
          if ((word0(rv) & Exp_mask) <= P*Exp_msk1)
          {
            word0(rv) += P*Exp_msk1;
            dval(rv) += adj*ulp(dval(rv));
            word0(rv) -= P*Exp_msk1;
          }
          else
#endif /*Sudden_Underflow*/
#endif /*Avoid_Underflow*/
            dval(rv) += adj*ulp(dval(rv));
        }
        break;
      }
      adj = ratio(delta, bs);
      if (adj < 1.)
        adj = 1.;
      if (adj <= 0x7ffffffe)
      {
        /* adj = rounding ? ceil(adj) : floor(adj); */
        y = (uint32_t)adj;
        if (y != adj)
        {
          if (!((rounding>>1) ^ dsign))
            y++;
          adj = y;
        }
      }
#ifdef Avoid_Underflow
      if (scale && (y = word0(rv) & Exp_mask) <= 2*P*Exp_msk1)
        word0(adj) += (2*P+1)*Exp_msk1 - y;
#else
#ifdef Sudden_Underflow
      if ((word0(rv) & Exp_mask) <= P*Exp_msk1)
      {
        word0(rv) += P*Exp_msk1;
        adj *= ulp(dval(rv));
        if (dsign)
          dval(rv) += adj;
        else
          dval(rv) -= adj;
        word0(rv) -= P*Exp_msk1;
        goto cont;
      }
#endif /*Sudden_Underflow*/
#endif /*Avoid_Underflow*/
      adj *= ulp(dval(rv));
      if (dsign)
        dval(rv) += adj;
      else
        dval(rv) -= adj;
      goto cont;
    }
#endif /*Honor_FLT_ROUNDS*/

    if (i < 0)
    {
      /* Error is less than half an ulp -- check for
       * special case of mantissa a power of two.
       */
      if (dsign || word1(rv) || word0(rv) & Bndry_mask
#ifdef IEEE_Arith
#ifdef Avoid_Underflow
       || (word0(rv) & Exp_mask) <= (2*P+1)*Exp_msk1
#else
       || (word0(rv) & Exp_mask) <= Exp_msk1
#endif
#endif
        )
      {
#ifdef SET_INEXACT
        if (!delta->x[0] && delta->wds <= 1)
          inexact = 0;
#endif
        break;
      }
      if (!delta->x[0] && delta->wds <= 1)
      {
        /* exact result */
#ifdef SET_INEXACT
        inexact = 0;
#endif
        break;
      }
      delta = BContext_lshift(&context, delta, Log2P);
      if (cmp(delta, bs) > 0) goto drop_down;
      break;
    }
    if (i == 0)
    {
      /* exactly half-way between */
      if (dsign)
      {
        if ((word0(rv) & Bndry_mask1) == Bndry_mask1 &&  word1(rv) == (
#ifdef Avoid_Underflow
          (scale && (y = word0(rv) & Exp_mask) <= 2*P*Exp_msk1)
          ? (0xffffffff & (0xffffffff << (2*P+1-(y>>Exp_shift)))) :
#endif
          0xffffffff))
        {
          /* boundary case -- increment exponent */
          word0(rv) = (word0(rv) & Exp_mask) + Exp_msk1
#ifdef IBM
            | Exp_msk1 >> 4
#endif
            ;
          word1(rv) = 0;
#ifdef Avoid_Underflow
          dsign = 0;
#endif
          break;
        }
      }
      else if (!(word0(rv) & Bndry_mask) && !word1(rv))
      {
drop_down:
        /* boundary case -- decrement exponent */
#ifdef Sudden_Underflow /*{{*/
        L = word0(rv) & Exp_mask;
#ifdef IBM
        if (L <  Exp_msk1)
#else
#ifdef Avoid_Underflow
        if (L <= (scale ? (2*P+1)*Exp_msk1 : Exp_msk1))
#else
        if (L <= Exp_msk1)
#endif /*Avoid_Underflow*/
#endif /*IBM*/
          goto undfl;
        L -= Exp_msk1;
#else /*Sudden_Underflow}{*/
#ifdef Avoid_Underflow
        if (scale)
        {
          L = word0(rv) & Exp_mask;
          if (L <= (2*P+1)*Exp_msk1)
          {
            if (L > (P+2)*Exp_msk1)
              /* round even ==> */
              /* accept rv */
              break;
            /* rv = smallest denormal */
            goto undfl;
          }
        }
#endif /*Avoid_Underflow*/
        L = (word0(rv) & Exp_mask) - Exp_msk1;
#endif /*Sudden_Underflow}}*/
        word0(rv) = L | Bndry_mask1;
        word1(rv) = 0xffffffff;
#ifdef IBM
        goto cont;
#else
        break;
#endif
      }
#ifndef ROUND_BIASED
      if (!(word1(rv) & LSB))
        break;
#endif
      if (dsign)
        dval(rv) += ulp(dval(rv));
#ifndef ROUND_BIASED
      else
      {
        dval(rv) -= ulp(dval(rv));
#ifndef Sudden_Underflow
        if (!dval(rv)) goto undfl;
#endif
      }
#ifdef Avoid_Underflow
      dsign = 1 - dsign;
#endif
#endif
      break;
    }
    if ((aadj = ratio(delta, bs)) <= 2.)
    {
      if (dsign) aadj = aadj1 = 1.;
      else if (word1(rv) || word0(rv) & Bndry_mask)
      {
#ifndef Sudden_Underflow
        if (word1(rv) == Tiny1 && !word0(rv)) goto undfl;
#endif
        aadj = 1.;
        aadj1 = -1.;
      }
      else
      {
        /* special case -- power of FLT_RADIX to be */
        /* rounded down... */

        if (aadj < 2./FLT_RADIX)
          aadj = 1./FLT_RADIX;
        else
          aadj *= 0.5;
        aadj1 = -aadj;
      }
    }
    else
    {
      aadj *= 0.5;
      aadj1 = dsign ? aadj : -aadj;
#ifdef Check_FLT_ROUNDS
      switch(Rounding)
      {
        case 2: /* towards +infinity */
          aadj1 -= 0.5;
          break;
        case 0: /* towards 0 */
        case 3: /* towards -infinity */
          aadj1 += 0.5;
      }
#else
      if (Flt_Rounds == 0) aadj1 += 0.5;
#endif /*Check_FLT_ROUNDS*/
    }
    y = word0(rv) & Exp_mask;

    /* Check for overflow */

    if (y == Exp_msk1*(DBL_MAX_EXP+Bias-1))
    {
      dval(rv0) = dval(rv);
      word0(rv) -= P*Exp_msk1;
      adj = aadj1 * ulp(dval(rv));
      dval(rv) += adj;
      if ((word0(rv) & Exp_mask) >= Exp_msk1*(DBL_MAX_EXP+Bias-P))
      {
        if (word0(rv0) == Big0 && word1(rv0) == Big1) goto ovfl;
        word0(rv) = Big0;
        word1(rv) = Big1;
        goto cont;
      }
      else
        word0(rv) += P*Exp_msk1;
    }
    else
    {
#ifdef Avoid_Underflow
      if (scale && y <= 2*P*Exp_msk1)
      {
        if (aadj <= 0x7fffffff)
        {
          if ((z = (int32_t)aadj) <= 0) z = 1;
          aadj = z;
          aadj1 = dsign ? aadj : -aadj;
        }
        word0(aadj1) += (2*P+1)*Exp_msk1 - y;
      }
      adj = aadj1 * ulp(dval(rv));
      dval(rv) += adj;
#else
#ifdef Sudden_Underflow
      if ((word0(rv) & Exp_mask) <= P*Exp_msk1)
      {
        dval(rv0) = dval(rv);
        word0(rv) += P*Exp_msk1;
        adj = aadj1 * ulp(dval(rv));
        dval(rv) += adj;
#ifdef IBM
        if ((word0(rv) & Exp_mask) <  P*Exp_msk1)
#else
        if ((word0(rv) & Exp_mask) <= P*Exp_msk1)
#endif
        {
          if (word0(rv0) == Tiny0 && word1(rv0) == Tiny1) goto undfl;
          word0(rv) = Tiny0;
          word1(rv) = Tiny1;
          goto cont;
        }
        else
          word0(rv) -= P*Exp_msk1;
      }
      else
      {
        adj = aadj1 * ulp(dval(rv));
        dval(rv) += adj;
      }
#else /*Sudden_Underflow*/
      /* Compute adj so that the IEEE rounding rules will
       * correctly round rv + adj in some half-way cases.
       * If rv * ulp(rv) is denormalized (i.e.,
       * y <= (P-1)*Exp_msk1), we must adjust aadj to avoid
       * trouble from bits lost to denormalization;
       * example: 1.2e-307 .
       */
      if (y <= (P-1)*Exp_msk1 && aadj > 1.)
      {
        aadj1 = (double)(int)(aadj + 0.5);
        if (!dsign) aadj1 = -aadj1;
      }
      adj = aadj1 * ulp(dval(rv));
      dval(rv) += adj;
#endif /*Sudden_Underflow*/
#endif /*Avoid_Underflow*/
    }
    z = word0(rv) & Exp_mask;
#ifndef SET_INEXACT
#ifdef Avoid_Underflow
    if (!scale)
#endif
    if (y == z)
    {
      /* Can we stop now? */
      L = (int32_t)aadj;
      aadj -= L;
      /* The tolerances below are conservative. */
      if (dsign || word1(rv) || word0(rv) & Bndry_mask)
      {
        if (aadj < .4999999 || aadj > .5000001) break;
      }
      else if (aadj < .4999999/FLT_RADIX)
      {
        break;
      }
    }
#endif
cont:
    BContext_bfree(&context, bb);
    BContext_bfree(&context, bd);
    BContext_bfree(&context, bs);
    BContext_bfree(&context, delta);
  }
#ifdef SET_INEXACT
  if (inexact)
  {
    if (!oldinexact)
    {
      word0(rv0) = Exp_1 + (70 << Exp_shift);
      word1(rv0) = 0;
      dval(rv0) += 1.;
    }
  }
  else if (!oldinexact)
  {
    clear_inexact();
  }
#endif
#ifdef Avoid_Underflow
  if (scale)
  {
    word0(rv0) = Exp_1 - 2*P*Exp_msk1;
    word1(rv0) = 0;
    dval(rv) *= dval(rv0);

    /* try to avoid the bug of testing an 8087 register value */
    if (word0(rv) == 0 && word1(rv) == 0) err = Error::Overflow;
  }
#endif /* Avoid_Underflow */
#ifdef SET_INEXACT
  if (inexact && !(word0(rv) & Exp_mask))
  {
    /* set underflow bit */
    dval(rv0) = 1e-300;
    dval(rv0) *= dval(rv0);
  }
#endif
retfree:
  BContext_bfree(&context, bb);
  BContext_bfree(&context, bd);
  BContext_bfree(&context, bs);
  BContext_bfree(&context, bd0);
  BContext_bfree(&context, delta);
ret:
  BContext_destroy(&context);

  if (end) *end = (sysuint_t)(s - sbegin);
  if (parserFlags) *parserFlags = flags;

  if (sign)
    *dst = -dval(rv);
  else
    *dst = dval(rv);
  return err;
}

// ============================================================================
// [Fog::StringUtil::Raw]
// ============================================================================

void copy(__G_CHAR* dst, const __G_CHAR* src, sysuint_t length)
{
#if __G_SIZE == 1
  memcpy((char*)dst, (const char*)src, length);
#else
  sysuint_t i;
  for (i = 0; i < length; i++) dst[i] = src[i];
#endif // __G_SIZE == 1
}

void move(__G_CHAR* dst, const __G_CHAR* src, sysuint_t length)
{
#if __G_SIZE == 1
  memmove((char*)dst, (const char*)src, length);
#else
  sysuint_t i;
  if (dst > src)
  {
    for (i = length - 1; i != (sysuint_t)-1; i--) dst[i] = src[i];
  }
  else
  {
    for (i = 0; i != length; i++) dst[i] = src[i];
  }
#endif // __G_SIZE == 1
}

void fill(__G_CHAR* dst, __G_CHAR ch, sysuint_t length)
{
#if __G_SIZE == 1
  memset((char*)dst, (int)ch.ch(), length);
#else
  sysuint_t i;
  for (i = 0; i < length; i++) dst[i] = ch;
#endif // __G_SIZE == 1
}

sysuint_t len(const __G_CHAR* str)
{
#if __G_SIZE == 1
  return strlen((char*)str);
#else
  const __G_CHAR* p = str;
  if (!p) return 0;

  while (*p) p++;
  return (sysuint_t)(p - str);
#endif // __G_SIZE == 1
}

sysuint_t nlen(const __G_CHAR* str, sysuint_t maxlen)
{
  const __G_CHAR* p = str;
  if (!p) return 0;
  const __G_CHAR* end = str + maxlen;

  while (p < end && *p) p++;
  return (sysuint_t)(p - str);
}

bool eq(const __G_CHAR* a, const __G_CHAR* b, sysuint_t length, uint cs)
{
  sysuint_t i;

  if (cs == CaseSensitive)
  {
    for (i = 0; i < length; i++)
    {
      if (a[i] != b[i]) return false;
    }
  }
  else
  {
    for (i = 0; i < length; i++)
    {
      if (a[i].toLower() != b[i].toLower()) return false;
    }
  }
  return true;
}

sysuint_t countOf(const __G_CHAR* str, sysuint_t length, __G_CHAR ch, uint cs)
{
  sysuint_t n = 0;
  sysuint_t i;

  if (cs == CaseSensitive)
  {
caseSensitiveLoop:
    for (i = 0; i < length; i++)
    {
      if (str[i] == ch) n++;
    }
  }
  else
  {
    __G_CHAR ch1 = ch.toLower();
    __G_CHAR ch2 = ch.toUpper();
    if (ch1 == ch2) goto caseSensitiveLoop;

    for (i = 0; i < length; i++)
    {
      if (str[i] == ch1 || str[i] == ch2) n++;
    }
  }

  return n;
}

sysuint_t indexOf(const __G_CHAR* str, sysuint_t length, __G_CHAR ch, uint cs)
{
  sysuint_t i;

  if (cs == CaseSensitive)
  {
caseSensitiveLoop:
    for (i = 0; i < length; i++)
    {
      if (str[i] == ch) return i;
    }
  }
  else
  {
    __G_CHAR ch1 = ch.toLower();
    __G_CHAR ch2 = ch.toUpper();
    if (ch1 == ch2) goto caseSensitiveLoop;

    for (i = 0; i < length; i++)
    {
      if (str[i] == ch1 || str[i] == ch2) return i;
    }
  }

  return InvalidIndex;
}

sysuint_t indexOfAny(const __G_CHAR* str, sysuint_t length, const __G_CHAR* ch, sysuint_t count, uint cs)
{
  if (count == DetectLength) count = len(ch);
  if (count == 0) 
    return InvalidIndex;
  else if (count == 1)
    return indexOf(str, length, ch[0], cs);

  sysuint_t i, j;

  if (cs == CaseSensitive)
  {
    for (i = 0; i < length; i++)
    {
      __G_CHAR cur = str[i];
      for (j = 0; j < count; j++)
      {
        if (cur == ch[j]) return i;
      }
    }
  }
  else 
  {
    for (i = 0; i < length; i++)
    {
      __G_CHAR cur1 = str[i].toLower();
      __G_CHAR cur2 = str[i].toUpper();
      for (j = 0; j < count; j++)
      {
        if (cur1 == ch[j] || cur2 == ch[j]) return i;
      }
    }
  }

  return InvalidIndex;
}

sysuint_t lastIndexOf(const __G_CHAR* str, sysuint_t length, __G_CHAR ch, uint cs)
{
  sysuint_t i;

  if (cs == CaseSensitive)
  {
caseSensitiveLoop:
    for (i = length - 1; i < (sysuint_t)-1; i--)
    {
      if (str[i] == ch) return i;
    }
  }
  else
  {
    __G_CHAR ch1 = ch.toLower();
    __G_CHAR ch2 = ch.toUpper();
    if (ch1 == ch2) goto caseSensitiveLoop;

    for (i = length - 1; i < (sysuint_t)-1; i--)
    {
      if (str[i] == ch1 || str[i] == ch2) return i;
    }
  }

  return InvalidIndex;
}

sysuint_t lastIndexOfAny(const __G_CHAR* str, sysuint_t length, const __G_CHAR* ch, sysuint_t count, uint cs)
{
  if (count == DetectLength) count = len(ch);
  if (count == 0) 
    return InvalidIndex;
  else if (count == 1)
    return lastIndexOf(str, length, ch[0], cs);

  sysuint_t i, j;

  if (cs == CaseSensitive)
  {
    for (i = length - 1; i != (sysuint_t)-1; i--)
    {
      __G_CHAR cur = str[i];
      for (j = 0; j < count; j++)
      {
        if (cur == ch[j]) return i;
      }
    }
  }
  else 
  {
    for (i = length - 1; i != (sysuint_t)-1; i--)
    {
      __G_CHAR cur1 = str[i].toLower();
      __G_CHAR cur2 = str[i].toUpper();
      for (j = 0; j < count; j++)
      {
        if (cur1 == ch[j] || cur2 == ch[j]) return i;
      }
    }
  }

  return InvalidIndex;
}

} // StringUtil namespace
} // Fog namespace

#undef __G_FRIEND
#undef __G_CHAR

// [Generator]
#endif // __G_GENERATE
