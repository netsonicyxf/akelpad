/*****************************************************************
 *              String functions header v3.3                     *
 *                                                               *
 * 2010 Shengalts Aleksander aka Instructor (Shengalts@mail.ru)  *
 *                                                               *
 *                                                               *
 *Functions:                                                     *
 * WideCharLower, WideCharUpper, xmemcpy, xmemcmp, xmemset,      *
 * xstrcmpA, xstrcmpW, xstrcmpiA, xstrcmpiW,                     *
 * xstrcmpnA, xstrcmpnW, xstrcmpinA, xstrcmpinW,                 *
 * xstrcpyA, xstrcpyW, xstrcpynA, xstrcpynW,                     *
 * xstrstrA, xstrstrW, StrReplaceA, StrReplaceW,                 *
 * GetOptionsA, GetOptionsW, WordFindA, WordFindW                *
 *                                                               *
 *****************************************************************/

#ifndef _STRFUNC_H_
#define _STRFUNC_H_

wchar_t WideCharLower(wchar_t c);
wchar_t WideCharUpper(wchar_t c);
void* xmemcpy(void *dest, const void *src, unsigned int count);
int xmemcmp(const void *buf1, const void *buf2, unsigned int count);
void* xmemset(void *dest, int c, unsigned int count);
int xstrcmpA(const char *pString1, const char *pString2);
int xstrcmpW(const wchar_t *wpString1, const wchar_t *wpString2);
int xstrcmpiA(const char *pString1, const char *pString2);
int xstrcmpiW(const wchar_t *wpString1, const wchar_t *wpString2);
int xstrcmpnA(const char *pString1, const char *pString2, DWORD dwMaxLength);
int xstrcmpnW(const wchar_t *wpString1, const wchar_t *wpString2, DWORD dwMaxLength);
int xstrcmpinA(const char *pString1, const char *pString2, DWORD dwMaxLength);
int xstrcmpinW(const wchar_t *wpString1, const wchar_t *wpString2, DWORD dwMaxLength);
int xstrcpyA(char *pString1, const char *pString2);
int xstrcpyW(wchar_t *wpString1, const wchar_t *wpString2);
int xstrcpynA(char *pString1, const char *pString2, unsigned int nMaxLength);
int xstrcpynW(wchar_t *wpString1, const wchar_t *wpString2, unsigned int nMaxLength);
BOOL xstrstrA(const char *pText, DWORD dwTextLen, const char *pStr, BOOL bSensitive, char **pStrBegin, char **pStrEnd);
BOOL xstrstrW(const wchar_t *wpText, DWORD dwTextLen, const wchar_t *wpStr, BOOL bSensitive, wchar_t **wpStrBegin, wchar_t **wpStrEnd);
int StrReplaceA(const char *pText, const char *pIt, const char *pWith, BOOL bSensitive, char *szResult, int *nMaxResult);
int StrReplaceW(const wchar_t *wpText, const wchar_t *wpIt, const wchar_t *wpWith, BOOL bSensitive, wchar_t *wszResult, int *nMaxResult);
int GetOptionsA(const char *pLine, const char *pOption, BOOL bSensitive, char *szResult, int nMaxResult);
int GetOptionsW(const wchar_t *wpLine, const wchar_t *wpOption, BOOL bSensitive, wchar_t *wszResult, int nMaxResult);
int WordFindA(const char *pText, const char *pDelim, int nNumber, const char *pOption, BOOL bSensitive, char *szResult, int *nMaxResult, char **ppResult);
int WordFindW(const wchar_t *wpText, const wchar_t *wpDelim, int nNumber, const wchar_t *wpOption, BOOL bSensitive, wchar_t *wszResult, int *nMaxResult, wchar_t **wppResult);

#endif


/********************************************************************
 *
 *  WideCharLower
 *
 *Translate wide character to lowercase.
 *
 * [in] wchar_t c  Unicode character.
 *
 *Returns: lowercase unicode character
 ********************************************************************/
#if defined WideCharLower || defined ALLSTRFUNC
#define WideCharLower_INCLUDED
#undef WideCharLower
wchar_t WideCharLower(wchar_t c)
{
  if (c < 0x100)
  {
    if ((c >= 0x0041 && c <= 0x005a) ||
        (c >= 0x00c0 && c <= 0x00de))
      return (c + 0x20);

    if (c == 0x00b5)
      return 0x03bc;

    return c;
  }
  else if (c < 0x300)
  {
    if ((c >= 0x0100 && c <= 0x012e) ||
        (c >= 0x0132 && c <= 0x0136) ||
        (c >= 0x014a && c <= 0x0176) ||
        (c >= 0x01de && c <= 0x01ee) ||
        (c >= 0x01f8 && c <= 0x021e) ||
        (c >= 0x0222 && c <= 0x0232))
    {
      if (!(c & 0x01))
        return (c + 1);
      return c;
    }

    if ((c >= 0x0139 && c <= 0x0147) ||
        (c >= 0x01cd && c <= 0x91db))
    {
      if (c & 0x01)
        return (c + 1);
      return c;
    }

    if (c >= 0x178 && c <= 0x01f7)
    {
      wchar_t k;

      switch (c)
      {
        case 0x0178:
          k=0x00ff;
          break;
        case 0x0179:
        case 0x017b:
        case 0x017d:
        case 0x0182:
        case 0x0184:
        case 0x0187:
        case 0x018b:
        case 0x0191:
        case 0x0198:
        case 0x01a0:
        case 0x01a2:
        case 0x01a4:
        case 0x01a7:
        case 0x01ac:
        case 0x01af:
        case 0x01b3:
        case 0x01b5:
        case 0x01b8:
        case 0x01bc:
        case 0x01c5:
        case 0x01c8:
        case 0x01cb:
        case 0x01cd:
        case 0x01cf:
        case 0x01d1:
        case 0x01d3:
        case 0x01d5:
        case 0x01d7:
        case 0x01d9:
        case 0x01db:
        case 0x01f2:
        case 0x01f4:
          k=c + 1;
          break;
        case 0x017f:
          k=0x0073;
          break;
        case 0x0181:
          k=0x0253;
          break;
        case 0x0186:
          k=0x0254;
          break;
        case 0x0189:
          k=0x0256;
          break;
        case 0x018a:
          k=0x0257;
          break;
        case 0x018e:
          k=0x01dd;
          break;
        case 0x018f:
          k=0x0259;
          break;
        case 0x0190:
          k=0x025b;
          break;
        case 0x0193:
          k=0x0260;
          break;
        case 0x0194:
          k=0x0263;
          break;
        case 0x0196:
          k=0x0269;
          break;
        case 0x0197:
          k=0x0268;
          break;
        case 0x019c:
          k=0x026f;
          break;
        case 0x019d:
          k=0x0272;
          break;
        case 0x019f:
          k=0x0275;
          break;
        case 0x01a6:
          k=0x0280;
          break;
        case 0x01a9:
          k=0x0283;
          break;
        case 0x01ae:
          k=0x0288;
          break;
        case 0x01b1:
          k=0x028a;
          break;
        case 0x01b2:
          k=0x028b;
          break;
        case 0x01b7:
          k=0x0292;
          break;
        case 0x01c4:
        case 0x01c7:
        case 0x01ca:
        case 0x01f1:
          k=c + 2;
          break;
        case 0x01f6:
          k=0x0195;
          break;
        case 0x01f7:
          k=0x01bf;
          break;
        default:
          k=0;
      }
      if (k != 0)
        return k;
    }
    if (c == 0x0220)
      return 0x019e;
  }
  else if (c < 0x0400)
  {
    if (c >= 0x0391 && c <= 0x03ab && c != 0x03a2)
      return (c + 0x20);
    if (c >= 0x03d8 && c <= 0x03ee && !(c & 0x01))
      return (c + 1);
    if (c >= 0x0386 && c <= 0x03f5)
    {
      wchar_t k;

      switch (c)
      {
        case 0x0386:
          k=0x03ac;
          break;
        case 0x0388:
          k=0x03ad;
          break;
        case 0x0389:
          k=0x03ae;
          break;
        case 0x038a:
          k=0x03af;
          break;
        case 0x038c:
          k=0x03cc;
          break;
        case 0x038e:
          k=0x03cd;
          break;
        case 0x038f:
          k=0x038f;
          break;
        case 0x03c2:
          k=0x03c3;
          break;
        case 0x03d0:
          k=0x03b2;
          break;
        case 0x03d1:
          k=0x03b8;
          break;
        case 0x03d5:
          k=0x03c6;
          break;
        case 0x03d6:
          k=0x03c0;
          break;
        case 0x03f0:
          k=0x03ba;
          break;
        case 0x03f1:
          k=0x03c1;
          break;
        case 0x03f2:
          k=0x03c3;
          break;
        case 0x03f4:
          k=0x03b8;
          break;
        case 0x03f5:
          k=0x03b5;
          break;
        default:
          k=0;
      }
      if (k != 0)
        return k;
    }
    if (c == 0x0345)
      return 0x03b9;
  }
  else if (c < 0x500)
  {
    if (c >= 0x0400 && c <= 0x040f)
      return (c + 0x50);

    if (c >= 0x0410 && c <= 0x042f)
      return (c + 0x20);

    if ((c >= 0x0460 && c <= 0x0480) ||
        (c >= 0x048a && c <= 0x04be) ||
        (c >= 0x04d0 && c <= 0x04f4) ||
        (c == 0x04f8))
    {
      if (!(c & 0x01))
        return (c + 1);
      return c;
    }

    if (c >= 0x04c1 && c <= 0x04cd)
    {
      if (c & 0x01)
        return (c + 1);
      return c;
    }
  }
  else if (c < 0x1f00)
  {
    if ((c >= 0x0500 && c <= 0x050e) ||
        (c >= 0x1e00 && c <= 0x1e94) ||
        (c >= 0x1ea0 && c <= 0x1ef8))
    {
      if (!(c & 0x01))
        return (c + 1);
      return c;
    }

    if (c >= 0x0531 && c <= 0x0556)
      return (c + 0x30);

    if (c == 0x1e9b)
      return 0x1e61;
  }
  else if (c < 0x2000)
  {
    if ((c >= 0x1f08 && c <= 0x1f0f) ||
        (c >= 0x1f18 && c <= 0x1f1d) ||
        (c >= 0x1f28 && c <= 0x1f2f) ||
        (c >= 0x1f38 && c <= 0x1f3f) ||
        (c >= 0x1f48 && c <= 0x1f4d) ||
        (c >= 0x1f68 && c <= 0x1f6f) ||
        (c >= 0x1f88 && c <= 0x1f8f) ||
        (c >= 0x1f98 && c <= 0x1f9f) ||
        (c >= 0x1fa8 && c <= 0x1faf))
      return (c - 0x08);

    if (c >= 0x1f59 && c <= 0x1f5f)
    {
      if (c & 0x01)
        return (c - 0x08);
      return c;
    }

    if (c >= 0x1fb8 && c <= 0x1ffc)
    {
      wchar_t k;

      switch (c)
      {
        case 0x1fb8:
        case 0x1fb9:
        case 0x1fd8:
        case 0x1fd9:
        case 0x1fe8:
        case 0x1fe9:
          k=c - 0x08;
          break;
        case 0x1fba:
        case 0x1fbb:
          k=c - 0x4a;
          break;
        case 0x1fbc:
          k=0x1fb3;
          break;
        case 0x1fbe:
          k=0x03b9;
          break;
        case 0x1fc8:
        case 0x1fc9:
        case 0x1fca:
        case 0x1fcb:
          k=c - 0x56;
          break;
        case 0x1fcc:
          k=0x1fc3;
          break;
        case 0x1fda:
        case 0x1fdb:
          k=c - 0x64;
          break;
        case 0x1fea:
        case 0x1feb:
          k=c - 0x70;
          break;
        case 0x1fec:
          k=0x1fe5;
          break;
        case 0x1ffa:
        case 0x1ffb:
          k=c - 0x7e;
          break;
        case 0x1ffc:
          k=0x1ff3;
          break;
        default:
          k=0;
      }
      if (k != 0)
        return k;
    }
  }
  else
  {
    if (c >= 0x2160 && c <= 0x216f)
      return (c + 0x10);

    if (c >= 0x24b6 && c <= 0x24cf)
      return (c + 0x1a);

    if (c >= 0xff21 && c <= 0xff3a)
      return (c + 0x20);

    //if (c >= 0x10400 && c <= 0x10425)
    //  return (c + 0x28);

    if (c == 0x2126)
      return 0x03c9;
    if (c == 0x212a)
      return 0x006b;
    if (c == 0x212b)
      return 0x00e5;
  }
  return c;
}
#endif

/********************************************************************
 *
 *  WideCharUpper
 *
 *Translate wide character to uppercase.
 *
 * [in] wchar_t c  Unicode character.
 *
 *Returns: uppercase unicode character
 ********************************************************************/
#if defined WideCharUpper || defined ALLSTRFUNC
#define WideCharUpper_INCLUDED
#undef WideCharUpper
wchar_t WideCharUpper(wchar_t c)
{
  if (c < 0x100)
  {
    if ((c >= 0x0061 && c <= 0x007a) ||
        (c >= 0x00e0 && c <= 0x00fe))
      return (c - 0x20);

    if (c == 0x00b5)
      return 0x039c;

    if (c == 0xff)
      return 0x0178;

    return c;
  }
  else if (c < 0x300)
  {
    if ((c >= 0x0101 && c <= 0x012f) ||
        (c >= 0x0133 && c <= 0x0137) ||
        (c >= 0x014b && c <= 0x0177) ||
        (c >= 0x01df && c <= 0x01ef) ||
        (c >= 0x01f9 && c <= 0x021f) ||
        (c >= 0x0223 && c <= 0x0233))
    {
      if (c & 0x01)
        return (c - 1);
      return c;
    }

    if ((c >= 0x013a && c <= 0x0148) ||
        (c >= 0x01ce && c <= 0x1dc))
    {
      if (!(c & 0x01))
        return (c - 1);
      return c;
    }

    if (c == 0x0131)
      return 0x0049;

    if (c == 0x017a || c == 0x017c || c == 0x017e)
      return (c - 1);

    if (c >= 0x017f && c <= 0x0292)
    {
      wchar_t k;

      switch (c)
      {
        case 0x017f:
          k=0x0053;
          break;
        case 0x0183:
          k=0x0182;
          break;
        case 0x0185:
          k=0x0184;
          break;
        case 0x0188:
          k=0x0187;
          break;
        case 0x018c:
          k=0x018b;
          break;
        case 0x0192:
          k=0x0191;
          break;
        case 0x0195:
          k=0x01f6;
          break;
        case 0x0199:
          k=0x0198;
          break;
        case 0x019e:
          k=0x0220;
          break;
        case 0x01a1:
        case 0x01a3:
        case 0x01a5:
        case 0x01a8:
        case 0x01ad:
        case 0x01b0:
        case 0x01b4:
        case 0x01b6:
        case 0x01b9:
        case 0x01bd:
        case 0x01c5:
        case 0x01c8:
        case 0x01cb:
        case 0x01f2:
        case 0x01f5:
          k=c - 1;
          break;
        case 0x01bf:
          k=0x01f7;
          break;
        case 0x01c6:
        case 0x01c9:
        case 0x01cc:
          k=c - 2;
          break;
        case 0x01dd:
          k=0x018e;
          break;
        case 0x01f3:
          k=0x01f1;
          break;
        case 0x0253:
          k=0x0181;
          break;
        case 0x0254:
          k=0x0186;
          break;
        case 0x0256:
          k=0x0189;
          break;
        case 0x0257:
          k=0x018a;
          break;
        case 0x0259:
          k=0x018f;
          break;
        case 0x025b:
          k=0x0190;
          break;
        case 0x0260:
          k=0x0193;
          break;
        case 0x0263:
          k=0x0194;
          break;
        case 0x0268:
          k=0x0197;
          break;
        case 0x0269:
          k=0x0196;
          break;
        case 0x026f:
          k=0x019c;
          break;
        case 0x0272:
          k=0x019d;
          break;
        case 0x0275:
          k=0x019f;
          break;
        case 0x0280:
          k=0x01a6;
          break;
        case 0x0283:
          k=0x01a9;
          break;
        case 0x0288:
          k=0x01ae;
          break;
        case 0x028a:
          k=0x01b1;
          break;
        case 0x028b:
          k=0x01b2;
          break;
        case 0x0292:
          k=0x01b7;
          break;
        default:
          k=0;
      }
      if (k != 0)
        return k;
    }
  }
  else if (c < 0x0400)
  {
    if (c == 0x03ac)
      return 0x0386;

    if ((c & 0xfff0) == 0x03a0 && c >= 0x03ad)
      return (c - 0x15);

    if (c >= 0x03b1 && c <= 0x03cb && c != 0x03c2)
      return (c - 0x20);

    if (c == 0x03c2)
      return 0x03a3;

    if (c >= 0x03cc && c <= 0x03f5)
    {
      wchar_t k;

      switch (c)
      {
        case 0x03cc:
          k=0x038c;
          break;
        case 0x03cd:
        case 0x03ce:
          k=c - 0x3f;
          break;
        case 0x03d0:
          k=0x0392;
          break;
        case 0x03d1:
          k=0x0398;
          break;
        case 0x03d5:
          k=0x03a6;
          break;
        case 0x03d6:
          k=0x03a0;
          break;
        case 0x03d9:
        case 0x03db:
        case 0x03dd:
        case 0x03df:
        case 0x03e1:
        case 0x03e3:
        case 0x03e5:
        case 0x03e7:
        case 0x03e9:
        case 0x03eb:
        case 0x03ed:
        case 0x03ef:
          k=c - 1;
          break;
        case 0x03f0:
          k=0x039a;
          break;
        case 0x03f1:
          k=0x03a1;
          break;
        case 0x03f2:
          k=0x03a3;
          break;
        case 0x03f5:
          k=0x0395;
          break;
        default:
          k=0;
      }
      if (k != 0)
        return k;
    }
  }
  else if (c < 0x500)
  {
    if (c >= 0x0450 && c <= 0x045f)
      return (c - 0x50);

    if (c >= 0x0430 && c <= 0x044f)
      return (c - 0x20);

    if ((c >= 0x0461 && c <= 0x0481) ||
        (c >= 0x048b && c <= 0x04bf) ||
        (c >= 0x04d1 && c <= 0x04f5))
    {
      if (c & 0x01)
        return (c - 1);
      return c;
    }

    if (c >= 0x04c2 && c <= 0x04ce)
    {
      if (!(c & 0x01))
        return (c - 1);
      return c;
    }

    if (c == 0x04f9)
      return 0x04f8;
  }
  else if (c < 0x1f00)
  {
    if ((c >= 0x0501 && c <= 0x050f) ||
        (c >= 0x1e01 && c <= 0x1e95) ||
        (c >= 0x1ea1 && c <= 0x1ef9))
    {
      if (c & 0x01)
        return (c - 1);
      return c;
    }

    if (c >= 0x0561 && c <= 0x0586)
      return (c - 0x30);

    if (c == 0x1e9b)
      return 0x1e60;
  }
  else if (c < 0x2000)
  {
    if ((c >= 0x1f00 && c <= 0x1f07) ||
        (c >= 0x1f10 && c <= 0x1f15) ||
        (c >= 0x1f20 && c <= 0x1f27) ||
        (c >= 0x1f30 && c <= 0x1f37) ||
        (c >= 0x1f40 && c <= 0x1f45) ||
        (c >= 0x1f60 && c <= 0x1f67) ||
        (c >= 0x1f80 && c <= 0x1f87) ||
        (c >= 0x1f90 && c <= 0x1f97) ||
        (c >= 0x1fa0 && c <= 0x1fa7))
      return (c + 0x08);

    if (c >= 0x1f51 && c <= 0x1f57 && (c & 0x01))
      return (c + 0x08);

    if (c >= 0x1f70 && c <= 0x1ff3)
    {
      wchar_t k;

      switch (c)
      {
        case 0x1fb0:
          k=0x1fb8;
          break;
        case 0x1fb1:
          k=0x1fb9;
          break;
        case 0x1f70:
          k=0x1fba;
          break;
        case 0x1f71:
          k=0x1fbb;
          break;
        case 0x1fb3:
          k=0x1fbc;
          break;
        case 0x1fbe:
          k=0x0399;
          break;
        case 0x1f72:
          k=0x1fc8;
          break;
        case 0x1f73:
          k=0x1fc9;
          break;
        case 0x1f74:
          k=0x1fca;
          break;
        case 0x1f75:
          k=0x1fcb;
          break;
        case 0x1fd0:
          k=0x1fd8;
          break;
        case 0x1fd1:
          k=0x1fd9;
          break;
        case 0x1f76:
          k=0x1fda;
          break;
        case 0x1f77:
          k=0x1fdb;
          break;
        case 0x1fe0:
          k=0x1fe8;
          break;
        case 0x1fe1:
          k=0x1fe9;
          break;
        case 0x1f7a:
          k=0x1fea;
          break;
        case 0x1f7b:
          k=0x1feb;
          break;
        case 0x1fe5:
          k=0x1fec;
          break;
        case 0x1f78:
          k=0x1ff8;
          break;
        case 0x1f79:
          k=0x1ff9;
          break;
        case 0x1f7c:
          k=0x1ffa;
          break;
        case 0x1f7d:
          k=0x1ffb;
          break;
        case 0x1ff3:
          k=0x1ffc;
          break;
        default:
          k=0;
      }
      if (k != 0)
        return k;
    }
  }
  else
  {
    if (c >= 0x2170 && c <= 0x217f)
      return (c - 0x10);

    if (c >= 0x24d0 && c <= 0x24e9)
      return (c - 0x1a);

    if (c >= 0xff41 && c <= 0xff5a)
      return (c - 0x20);

//    if (c >= 0x10428 && c <= 0x1044d)
//      return (c - 0x28);
  }
  return c;
}
#endif

/********************************************************************
 *
 *  xmemcpy
 *
 *Copies characters between buffers.
 *
 *[out] void *dest          New buffer.
 * [in] const void *src     Buffer to copy from.
 * [in] unsigned int count  Number of bytes to copy.
 *
 *Returns:  the value of dest.
 ********************************************************************/
#if defined xmemcpy || defined ALLSTRFUNC
#define xmemcpy_INCLUDED
#undef xmemcpy
void* xmemcpy(void *dest, const void *src, unsigned int count)
{
  unsigned char *byte_dest=(unsigned char *)dest;
  unsigned char *byte_src=(unsigned char *)src;

  if (byte_dest != byte_src)
    while (count--) *byte_dest++=*byte_src++;

  return dest;
}
#endif

/********************************************************************
 *
 *  xmemcmp
 *
 *Compare characters in two buffers.
 *
 *[in] const void *buf1    First buffer.
 *[in] const void *buf2    Second buffer.
 *[in] unsigned int count  Number of bytes.
 *
 *Returns:  -1 buf1 less than buf2
 *           0 buf1 identical to buf2
 *           1 buf1 greater than buf2
 ********************************************************************/
#if defined xmemcmp || defined ALLSTRFUNC
#define xmemcmp_INCLUDED
#undef xmemcmp
int xmemcmp(const void *buf1, const void *buf2, unsigned int count)
{
  unsigned char *byte_buf1=(unsigned char *)buf1;
  unsigned char *byte_buf2=(unsigned char *)buf2;

  if (byte_buf1 != byte_buf2)
  {
    while (count--)
    {
      if (*byte_buf1 == *byte_buf2)
      {
        ++byte_buf1;
        ++byte_buf2;
        continue;
      }
      else if (*byte_buf1 < *byte_buf2)
        return -1;
      else
        return 1;
    }
  }
  return 0;
}
#endif

/********************************************************************
 *
 *  xmemset
 *
 *Compare characters in two buffers.
 *
 *[out] void *dest          Pointer to destination.
 * [in] int c               Character to set.
 * [in] unsigned int count  Number of characters.
 *
 *Returns:  the value of dest.
 ********************************************************************/
#if defined xmemset || defined ALLSTRFUNC
#define xmemset_INCLUDED
#undef xmemset
void* xmemset(void *dest, int c, unsigned int count)
{
  unsigned char *byte_dest=(unsigned char *)dest;

  while (count--) *byte_dest++=c;

  return dest;
}
#endif

/********************************************************************
 *
 *  xstrcmpA
 *
 *Case sensitive comparison of two strings.
 *
 *[in] char *pString1  First string to compare.
 *[in] char *pString2  Second string to compare.
 *
 *Returns:  -1 string1 less than string2
 *           0 string1 identical to string2
 *           1 string1 greater than string2
 ********************************************************************/
#if defined xstrcmpA || defined ALLSTRFUNC
#define xstrcmpA_INCLUDED
#undef xstrcmpA
int xstrcmpA(const char *pString1, const char *pString2)
{
  while (*pString1)
  {
    if (*pString1 != *pString2)
      return ((DWORD)*pString1 > (DWORD)*pString2)?1:-1;

    ++pString1;
    ++pString2;
  }

  if (*pString1 == *pString2)
    return 0;
  return -1;
}
#endif

/********************************************************************
 *
 *  xstrcmpW
 *
 *Case sensitive comparison of two unicode strings.
 *
 *[in] wchar_t *wpString1  First string to compare.
 *[in] wchar_t *wpString2  Second string to compare.
 *
 *Returns:  -1 string1 less than string2
 *           0 string1 identical to string2
 *           1 string1 greater than string2
 ********************************************************************/
#if defined xstrcmpW || defined ALLSTRFUNC
#define xstrcmpW_INCLUDED
#undef xstrcmpW
int xstrcmpW(const wchar_t *wpString1, const wchar_t *wpString2)
{
  while (*wpString1)
  {
    if (*wpString1 != *wpString2)
      return ((DWORD)*wpString1 > (DWORD)*wpString2)?1:-1;

    ++wpString1;
    ++wpString2;
  }

  if (*wpString1 == *wpString2)
    return 0;
  return -1;
}
#endif

/********************************************************************
 *
 *  xstrcmpiA
 *
 *Case insensitive comparison of two strings.
 *
 *[in] char *pString1  First string to compare.
 *[in] char *pString2  Second string to compare.
 *
 *Returns:  -1 string1 less than string2
 *           0 string1 identical to string2
 *           1 string1 greater than string2
 ********************************************************************/
#if defined xstrcmpiA || defined ALLSTRFUNC
#define xstrcmpiA_INCLUDED
#undef xstrcmpiA
int xstrcmpiA(const char *pString1, const char *pString2)
{
  int nCompare;

  while (*pString1)
  {
    if (nCompare=(WORD)(DWORD)CharUpperA((char *)(DWORD)(WORD)*pString1) - (WORD)(DWORD)CharUpperA((char *)(DWORD)(WORD)*pString2))
      return (nCompare > 0)?1:-1;

    ++pString1;
    ++pString2;
  }

  if (*pString1 == *pString2)
    return 0;
  return -1;
}
#endif

/********************************************************************
 *
 *  xstrcmpiW
 *
 *Case insensitive comparison of two unicode strings.
 *
 *[in] wchar_t *wpString1  First string to compare.
 *[in] wchar_t *wpString2  Second string to compare.
 *
 *Returns:  -1 string1 less than string2
 *           0 string1 identical to string2
 *           1 string1 greater than string2
 *
 *Note:
 *  xstrcmpiW can be used on Win95/98/Me if WideCharLower or WideCharUpper defined.
 ********************************************************************/
#if defined xstrcmpiW || defined ALLSTRFUNC
#define xstrcmpiW_INCLUDED
#undef xstrcmpiW
int xstrcmpiW(const wchar_t *wpString1, const wchar_t *wpString2)
{
  int nCompare;

  while (*wpString1)
  {
    #if defined WideCharLower_INCLUDED
      if (nCompare=WideCharLower(*wpString1) - WideCharLower(*wpString2))
        return (nCompare > 0)?1:-1;
    #elif defined WideCharUpper_INCLUDED
      if (nCompare=WideCharUpper(*wpString1) - WideCharUpper(*wpString2))
        return (nCompare > 0)?1:-1;
    #else
      if (nCompare=(WORD)(DWORD)CharUpperW((wchar_t *)(DWORD)(WORD)*wpString1) - (WORD)(DWORD)CharUpperW((wchar_t *)(DWORD)(WORD)*wpString2))
        return (nCompare > 0)?1:-1;
    #endif

    ++wpString1;
    ++wpString2;
  }

  if (*wpString1 == *wpString2)
    return 0;
  return -1;
}
#endif

/********************************************************************
 *
 *  xstrcmpnA
 *
 *Case sensitive comparison specified number of characters of two strings.
 *
 *[in] char *pString1     First string to compare.
 *[in] char *pString2     Second string to compare.
 *[in] DWORD dwMaxLength  Number of characters to compare,
 *                         -1 compare until NULL character in pString1.
 *
 *Returns:  -1 string1 less than string2
 *           0 string1 identical to string2
 *           1 string1 greater than string2
 ********************************************************************/
#if defined xstrcmpnA || defined ALLSTRFUNC
#define xstrcmpnA_INCLUDED
#undef xstrcmpnA
int xstrcmpnA(const char *pString1, const char *pString2, DWORD dwMaxLength)
{
  DWORD dwCount=dwMaxLength;

  while (dwCount && *pString1)
  {
    if (*pString1 != *pString2)
      return ((DWORD)*pString1 > (DWORD)*pString2)?1:-1;
    ++pString1;
    ++pString2;
    --dwCount;
  }
  if (!dwCount) return 0;
  if (!*pString2) return 0;
  return (dwMaxLength == (DWORD)-1)?0:-1;
}
#endif

/********************************************************************
 *
 *  xstrcmpnW
 *
 *Case sensitive comparison specified number of characters of two unicode strings.
 *
 *[in] wchar_t *wpString1  First string to compare.
 *[in] wchar_t *wpString2  Second string to compare.
 *[in] DWORD dwMaxLength   Number of characters to compare,
 *                          -1 compare until NULL character in wpString1.
 *
 *Returns:  -1 string1 less than string2
 *           0 string1 identical to string2
 *           1 string1 greater than string2
 ********************************************************************/
#if defined xstrcmpnW || defined ALLSTRFUNC
#define xstrcmpnW_INCLUDED
#undef xstrcmpnW
int xstrcmpnW(const wchar_t *wpString1, const wchar_t *wpString2, DWORD dwMaxLength)
{
  DWORD dwCount=dwMaxLength;

  while (dwCount && *wpString1)
  {
    if (*wpString1 != *wpString2)
      return ((DWORD)*wpString1 > (DWORD)*wpString2)?1:-1;

    ++wpString1;
    ++wpString2;
    --dwCount;
  }
  if (!dwCount) return 0;
  if (!*wpString2) return 0;
  return (dwMaxLength == (DWORD)-1)?0:-1;
}
#endif

/********************************************************************
 *
 *  xstrcmpinA
 *
 *Case insensitive comparison specified number of characters of two strings.
 *
 *[in] char *pString1     First string to compare.
 *[in] char *pString2     Second string to compare.
 *[in] DWORD dwMaxLength  Number of characters to compare,
 *                         -1 compare until NULL character in pString1.
 *
 *Returns:  -1 string1 less than string2
 *           0 string1 identical to string2
 *           1 string1 greater than string2
 ********************************************************************/
#if defined xstrcmpinA || defined ALLSTRFUNC
#define xstrcmpinA_INCLUDED
#undef xstrcmpinA
int xstrcmpinA(const char *pString1, const char *pString2, DWORD dwMaxLength)
{
  DWORD dwCount=dwMaxLength;
  int nCompare;

  while (dwCount && *pString1)
  {
    if (nCompare=(WORD)(DWORD)CharUpperA((char *)(DWORD)(WORD)*pString1) - (WORD)(DWORD)CharUpperA((char *)(DWORD)(WORD)*pString2))
      return (nCompare > 0)?1:-1;

    ++pString1;
    ++pString2;
    --dwCount;
  }
  if (!dwCount) return 0;
  if (!*pString2) return 0;
  return (dwMaxLength == (DWORD)-1)?0:-1;
}
#endif

/********************************************************************
 *
 *  xstrcmpinW
 *
 *Case insensitive comparison specified number of characters of two unicode strings.
 *
 *[in] wchar_t *wpString1  First string to compare.
 *[in] wchar_t *wpString2  Second string to compare.
 *[in] DWORD dwMaxLength   Number of characters to compare,
 *                          -1 compare until NULL character in wpString1.
 *
 *Returns:  -1 string1 less than string2
 *           0 string1 identical to string2
 *           1 string1 greater than string2
 *
 *Note:
 *  xstrcmpinW can be used on Win95/98/Me if WideCharLower or WideCharUpper defined.
 ********************************************************************/
#if defined xstrcmpinW || defined ALLSTRFUNC
#define xstrcmpinW_INCLUDED
#undef xstrcmpinW
int xstrcmpinW(const wchar_t *wpString1, const wchar_t *wpString2, DWORD dwMaxLength)
{
  DWORD dwCount=dwMaxLength;
  int nCompare;

  while (dwCount && *wpString1)
  {
    #if defined WideCharLower_INCLUDED
      if (nCompare=WideCharLower(*wpString1) - WideCharLower(*wpString2))
        return (nCompare > 0)?1:-1;
    #elif defined WideCharUpper_INCLUDED
      if (nCompare=WideCharUpper(*wpString1) - WideCharUpper(*wpString2))
        return (nCompare > 0)?1:-1;
    #else
      if (nCompare=(WORD)(DWORD)CharUpperW((wchar_t *)(DWORD)(WORD)*wpString1) - (WORD)(DWORD)CharUpperW((wchar_t *)(DWORD)(WORD)*wpString2))
        return (nCompare > 0)?1:-1;
    #endif

    ++wpString1;
    ++wpString2;
    --dwCount;
  }
  if (!dwCount) return 0;
  if (!*wpString2) return 0;
  return (dwMaxLength == (DWORD)-1)?0:-1;
}
#endif

/********************************************************************
 *
 *  xstrcpyA
 *
 *Copies a string into a buffer.
 *
 *[in] char *pString1  Pointer to a buffer into which the function copies characters.
 *                      The buffer must be large enough to contain the string,
 *                      including the terminating null character.
 *[in] char *pString2  Pointer to a null-terminated string from which the function copies characters.
 *
 *Returns:  number of characters copied, not including the terminating null character.
 ********************************************************************/
#if defined xstrcpyA || defined ALLSTRFUNC
#define xstrcpyA_INCLUDED
#undef xstrcpyA
int xstrcpyA(char *pString1, const char *pString2)
{
  char *pDest=pString1;
  char *pSrc=(char *)pString2;

  if (pDest != pSrc)
  {
    while (*pSrc)
      *pDest++=*pSrc++;
    *pDest=L'\0';
  }
  return pDest - pString1;
}
#endif

/********************************************************************
 *
 *  xstrcpyW
 *
 *Copies a unicode string into a buffer.
 *
 *[in] wchar_t *wpString1  Pointer to a buffer into which the function copies characters.
 *                          The buffer must be large enough to contain the string,
 *                          including the terminating null character.
 *[in] wchar_t *wpString2  Pointer to a null-terminated string from which the function copies characters.
 *
 *Returns:  number of characters copied, not including the terminating null character.
 ********************************************************************/
#if defined xstrcpyW || defined ALLSTRFUNC
#define xstrcpyW_INCLUDED
#undef xstrcpyW
int xstrcpyW(wchar_t *wpString1, const wchar_t *wpString2)
{
  wchar_t *wpDest=wpString1;
  wchar_t *wpSrc=(wchar_t *)wpString2;

  if (wpDest != wpSrc)
  {
    while (*wpSrc)
      *wpDest++=*wpSrc++;
    *wpDest=L'\0';
  }
  return wpDest - wpString1;
}
#endif

/********************************************************************
 *
 *  xstrcpynA
 *
 *Copies a specified number of characters from a source string into a buffer.
 *
 *[in] char *pString1           Pointer to a buffer into which the function copies characters.
 *                               The buffer must be large enough to contain the number of TCHAR values specified by nMaxLength,
 *                               including room for a terminating null character.
 *[in] char *pString2           Pointer to a null-terminated string from which the function copies characters.
 *[in] unsigned int nMaxLength  Specifies the number of TCHAR values to be copied from the string pointed to by pString2 into the buffer pointed to by pString1,
 *                               including a terminating null character.
 *
 *Returns:  number of characters copied, not including the terminating null character.
 ********************************************************************/
#if defined xstrcpynA || defined ALLSTRFUNC
#define xstrcpynA_INCLUDED
#undef xstrcpynA
int xstrcpynA(char *pString1, const char *pString2, unsigned int nMaxLength)
{
  char *pDest=pString1;
  char *pSrc=(char *)pString2;

  if (pDest != pSrc)
  {
    while (*pSrc && --nMaxLength)
      *pDest++=*pSrc++;
    *pDest=L'\0';
  }
  return pDest - pString1;
}
#endif

/********************************************************************
 *
 *  xstrcpynW
 *
 *Copies a specified number of unicode characters from a source string into a buffer.
 *
 *[in] wchar_t *wpString1       Pointer to a buffer into which the function copies characters.
 *                               The buffer must be large enough to contain the number of TCHAR values specified by nMaxLength,
 *                               including room for a terminating null character.
 *[in] wchar_t *wpString2       Pointer to a null-terminated string from which the function copies characters.
 *[in] unsigned int nMaxLength  Specifies the number of TCHAR values to be copied from the string pointed to by wpString2 into the buffer pointed to by wpString1,
 *                               including a terminating null character.
 *
 *Returns:  number of characters copied, not including the terminating null character.
 ********************************************************************/
#if defined xstrcpynW || defined ALLSTRFUNC
#define xstrcpynW_INCLUDED
#undef xstrcpynW
int xstrcpynW(wchar_t *wpString1, const wchar_t *wpString2, unsigned int nMaxLength)
{
  wchar_t *wpDest=wpString1;
  wchar_t *wpSrc=(wchar_t *)wpString2;

  if (wpDest != wpSrc)
  {
    while (*wpSrc && --nMaxLength)
      *wpDest++=*wpSrc++;
    *wpDest=L'\0';
  }
  return wpDest - wpString1;
}
#endif

/********************************************************************
 *
 *  xstrstrA
 *
 *Find substring in string.
 *
 * [in] const char *pText  Text.
 * [in] DWORD dwTextLen    Text length, -1 search until NULL character in pText.
 * [in] const char *pStr   Find it.
 * [in] BOOL bSensitive    TRUE   case sensitive.
 *                         FALSE  case insensitive.
 *[out] char **pStrBegin   Pointer to the first char of pStr, can be NULL.
 *[out] char **pStrEnd     Pointer to the first char after pStr, can be NULL.
 *
 *Returns:  TRUE  pStr is founded
 *          FALSE pStr isn't founded
 ********************************************************************/
#if defined xstrstrA || defined ALLSTRFUNC
#define xstrstrA_INCLUDED
#undef xstrstrA
BOOL xstrstrA(const char *pText, DWORD dwTextLen, const char *pStr, BOOL bSensitive, char **pStrBegin, char **pStrEnd)
{
  const char *pTextCount;
  const char *pTextMax=pText + dwTextLen;
  const char *pStrCount;

  for (; ; ++pText)
  {
    if (dwTextLen == (DWORD)-1)
    {
      if (!*pText)
        break;
    }
    else
    {
      if ((DWORD)pText >= (DWORD)pTextMax)
        break;
    }

    for (pTextCount=pText, pStrCount=pStr;
          *pTextCount == *pStrCount ||
          (bSensitive == FALSE && (char)(WORD)(DWORD)CharUpperA((char *)(DWORD)(WORD)*pTextCount) == (char)(WORD)(DWORD)CharUpperA((char *)(DWORD)(WORD)*pStrCount));
         ++pTextCount)
    {
      if (!*++pStrCount)
      {
        if (pStrBegin) *pStrBegin=(char *)pText;
        if (pStrEnd) *pStrEnd=(char *)(pTextCount + 1);
        return TRUE;
      }
    }
  }
  return FALSE;
}
#endif

/********************************************************************
 *
 *  xstrstrW
 *
 *Find substring in unicode string.
 *
 * [in] const wchar_t *wpText  Text.
 * [in] DWORD dwTextLen        Text length, -1 search until NULL character in wpText.
 * [in] const wchar_t *wpStr   Find it.
 * [in] BOOL bSensitive        TRUE   case sensitive.
 *                             FALSE  case insensitive.
 *[out] wchar_t **wpStrBegin   Pointer to the first char of wpStr, can be NULL.
 *[out] wchar_t **wpStrEnd     Pointer to the first char after wpStr, can be NULL.
 *
 *Returns:  TRUE  wpStr is founded
 *          FALSE wpStr isn't founded
 *
 *Note:
 *  xstrstrW can be used on Win95/98/Me if WideCharLower or WideCharUpper defined.
 ********************************************************************/
#if defined xstrstrW || defined ALLSTRFUNC
#define xstrstrW_INCLUDED
#undef xstrstrW
BOOL xstrstrW(const wchar_t *wpText, DWORD dwTextLen, const wchar_t *wpStr, BOOL bSensitive, wchar_t **wpStrBegin, wchar_t **wpStrEnd)
{
  const wchar_t *wpTextCount;
  const wchar_t *wpTextMax=wpText + dwTextLen;
  const wchar_t *wpStrCount;

  for (; ; ++wpText)
  {
    if (dwTextLen == (DWORD)-1)
    {
      if (!*wpText)
        break;
    }
    else
    {
      if ((DWORD)wpText >= (DWORD)wpTextMax)
        break;
    }

    for (wpTextCount=wpText, wpStrCount=wpStr;
          *wpTextCount == *wpStrCount ||
          #if defined WideCharLower_INCLUDED
            (bSensitive == FALSE && WideCharLower(*wpTextCount) == WideCharLower(*wpStrCount));
          #elif defined WideCharUpper_INCLUDED
            (bSensitive == FALSE && WideCharUpper(*wpTextCount) == WideCharUpper(*wpStrCount));
          #else
            (bSensitive == FALSE && (wchar_t)(WORD)(DWORD)CharUpperW((wchar_t *)(DWORD)(WORD)*wpTextCount) == (wchar_t)(WORD)(DWORD)CharUpperW((wchar_t *)(DWORD)(WORD)*wpStrCount));
          #endif
         ++wpTextCount)
    {
      if (!*++wpStrCount)
      {
        if (wpStrBegin) *wpStrBegin=(wchar_t *)wpText;
        if (wpStrEnd) *wpStrEnd=(wchar_t *)(wpTextCount + 1);
        return TRUE;
      }
    }
  }
  return FALSE;
}
#endif

/********************************************************************
 *
 *  StrReplaceA
 *
 *Replace substring in string.
 *
 * [in] const char *pText  Text.
 * [in] const char *pIt    Replace it.
 * [in] const char *pWith  Replace with.
 * [in] BOOL bSensitive    TRUE   case sensitive.
 *                         FALSE  case insensitive.
 *[out] char *szResult     Output, can be NULL.
 *[out] int *nMaxResult    Contains the length of the result string,
 *                          including the terminating null character,
 *                          can be NULL.
 *
 *Returns:  Number of changes
 ********************************************************************/
#if defined StrReplaceA || defined ALLSTRFUNC
#define StrReplaceA_INCLUDED
#undef StrReplaceA
int StrReplaceA(const char *pText, const char *pIt, const char *pWith, BOOL bSensitive, char *szResult, int *nMaxResult)
{
  const char *pTextCount;
  const char *pItCount;
  char *pResult=szResult;
  int nWithLen=lstrlenA(pWith);
  int nChanges=0;
  int i;

  for (pTextCount=pText; *pText; ++pText)
  {
    for (pTextCount=pText, pItCount=pIt;
          *pTextCount == *pItCount ||
          (bSensitive == FALSE && (char)(WORD)(DWORD)CharUpperA((char *)(DWORD)(WORD)*pTextCount) == (char)(WORD)(DWORD)CharUpperA((char *)(DWORD)(WORD)*pItCount));
         ++pTextCount)
    {
      if (!*++pItCount)
      {
        if (szResult)
        {
          for (i=0; i < nWithLen; ++i)
            *pResult++=pWith[i];
        }
        else pResult+=nWithLen;

        pText=pTextCount + 1;
        pItCount=pIt;
        ++nChanges;
        if (!*pText) goto End;
      }
    }
    if (szResult) *pResult=*pText;
    ++pResult;
  }

  End:
  if (szResult) *pResult=*pText;
  if (nMaxResult) *nMaxResult=(pResult - szResult) + 1;
  return nChanges;
}
#endif

/********************************************************************
 *
 *  StrReplaceW
 *
 *Replace substring in unicode string.
 *
 * [in] const wchar_t *wpText  Text.
 * [in] const wchar_t *wpIt    Replace it.
 * [in] const wchar_t *wpWith  Replace with.
 * [in] BOOL bSensitive        TRUE   case sensitive.
 *                             FALSE  case insensitive.
 *[out] wchar_t *wszResult     Output, can be NULL.
 *[out] int *nMaxResult        Contains the length of the result string,
 *                              including the terminating null character,
 *                              can be NULL.
 *
 *Returns:  Number of changes
 *
 *Note:
 *  StrReplaceW can be used on Win95/98/Me if WideCharLower or WideCharUpper defined.
 ********************************************************************/
#if defined StrReplaceW || defined ALLSTRFUNC
#define StrReplaceW_INCLUDED
#undef StrReplaceW
int StrReplaceW(const wchar_t *wpText, const wchar_t *wpIt, const wchar_t *wpWith, BOOL bSensitive, wchar_t *wszResult, int *nMaxResult)
{
  const wchar_t *wpTextCount;
  const wchar_t *wpItCount;
  wchar_t *wpResult=wszResult;
  int nWithLen=lstrlenW(wpWith);
  int nChanges=0;
  int i;

  for (wpTextCount=wpText; *wpText; ++wpText)
  {
    for (wpTextCount=wpText, wpItCount=wpIt;
          *wpTextCount == *wpItCount ||
          #if defined WideCharLower_INCLUDED
            (bSensitive == FALSE && WideCharLower(*wpTextCount) == WideCharLower(*wpItCount));
          #elif defined WideCharUpper_INCLUDED
            (bSensitive == FALSE && WideCharUpper(*wpTextCount) == WideCharUpper(*wpItCount));
          #else
            (bSensitive == FALSE && (wchar_t)(WORD)(DWORD)CharUpperW((wchar_t *)(DWORD)(WORD)*wpTextCount) == (wchar_t)(WORD)(DWORD)CharUpperW((wchar_t *)(DWORD)(WORD)*wpItCount));
          #endif
         ++wpTextCount)
    {
      if (!*++wpItCount)
      {
        if (wszResult)
        {
          for (i=0; i < nWithLen; ++i)
            *wpResult++=wpWith[i];
        }
        else wpResult+=nWithLen;

        wpText=wpTextCount + 1;
        wpItCount=wpIt;
        ++nChanges;
        if (!*wpText) goto End;
      }
    }
    if (wszResult) *wpResult=*wpText;
    ++wpResult;
  }

  End:
  if (wszResult) *wpResult=*wpText;
  if (nMaxResult) *nMaxResult=(wpResult - wszResult) + 1;
  return nChanges;
}
#endif

/********************************************************************
 *
 *  GetOptionsA
 *
 *Gets option string from parameters line.
 *
 * [in] const char *pLine    Parameters line.
 * [in] const char *pOption  Option.
 * [in] BOOL bSensitive      TRUE   case sensitive.
 *                           FALSE  case insensitive.
 *[out] char *szResult       Output, can be NULL.
 * [in] int nMaxResult       Output buffer size.
 *
 *Returns:  length of the string copied to szResult,
 *          including the terminating null character
 ********************************************************************/
#if defined GetOptionsA || defined ALLSTRFUNC
#define GetOptionsA_INCLUDED
#undef GetOptionsA
int GetOptionsA(const char *pLine, const char *pOption, BOOL bSensitive, char *szResult, int nMaxResult)
{
  const char *pLineStart;
  const char *pLineEnd;
  const char *pOptionCount;
  const char *pOptionString=NULL;
  char chQuote='\0';
  char chDelimiter=*pOption++;
  int nBytes=0;

  for (pLineStart=pLineEnd=pLine; *pLineStart && *pLineEnd; ++pLineStart)
  {
    if (chQuote == '\0' && *pLineStart != '\"' && *pLineStart != '\'' && *pLineStart != '`')
    {
      if (*pLineStart == chDelimiter)
      {
        if (pOptionString) break;

        for (pLineEnd=pLineStart + 1, pOptionCount=pOption;
              *pLineEnd &&
              (*pLineEnd == *pOptionCount ||
              (bSensitive == FALSE && (char)(WORD)(DWORD)CharUpperA((char *)(DWORD)(WORD)*pLineEnd) == (char)(WORD)(DWORD)CharUpperA((char *)(DWORD)(WORD)*pOptionCount)));
             ++pLineEnd)
        {
          if (!*++pOptionCount)
          {
            pLineStart=pLineEnd;
            pOptionString=pLineEnd + 1;
            break;
          }
        }
      }
    }
    else if (chQuote == '\0')
      chQuote=*pLineStart;
    else if (*pLineStart == chQuote)
      chQuote='\0';
  }
  if (pOptionString)
  {
    while (*--pLineStart == ' ' || *pLineStart == '\t');

    if (*pOptionString == *pLineStart && (*pOptionString == '\"' || *pOptionString == '\'' || *pOptionString == '`'))
    {
      ++pOptionString, --pLineStart;
    }
    nBytes=pLineStart - pOptionString + 2;
    if (szResult)
    {
      if (nMaxResult < nBytes) nBytes=nMaxResult;
      lstrcpynA(szResult, pOptionString, nBytes);
    }
  }
  return nBytes;
}
#endif

/********************************************************************
 *
 *  GetOptionsW
 *
 *Gets option string from unicode parameters line.
 *
 * [in] const wchar_t *wpLine    Parameters line.
 * [in] const wchar_t *wpOption  Option.
 * [in] BOOL bSensitive          TRUE   case sensitive.
 *                               FALSE  case insensitive.
 *[out] wchar_t *wszResult       Output, can be NULL.
 * [in] int nMaxResult           Output buffer size.
 *
 *Returns:  length of the string copied to wszResult,
 *          including the terminating null character
 ********************************************************************/
#if defined GetOptionsW || defined ALLSTRFUNC
#define GetOptionsW_INCLUDED
#undef GetOptionsW
int GetOptionsW(const wchar_t *wpLine, const wchar_t *wpOption, BOOL bSensitive, wchar_t *wszResult, int nMaxResult)
{
  const wchar_t *wpLineStart;
  const wchar_t *wpLineEnd;
  const wchar_t *wpOptionCount;
  const wchar_t *wpOptionString=NULL;
  wchar_t wchQuote='\0';
  wchar_t wchDelimiter=*wpOption++;
  int nBytes=0;

  for (wpLineStart=wpLineEnd=wpLine; *wpLineStart && *wpLineEnd; ++wpLineStart)
  {
    if (wchQuote == '\0' && *wpLineStart != '\"' && *wpLineStart != '\'' && *wpLineStart != '`')
    {
      if (*wpLineStart == wchDelimiter)
      {
        if (wpOptionString) break;

        for (wpLineEnd=wpLineStart + 1, wpOptionCount=wpOption;
              *wpLineEnd &&
              (*wpLineEnd == *wpOptionCount ||
               #if defined WideCharLower_INCLUDED
                 (bSensitive == FALSE && WideCharLower(*wpLineEnd) == WideCharLower(*wpOptionCount)));
               #elif defined WideCharUpper_INCLUDED
                 (bSensitive == FALSE && WideCharUpper(*wpLineEnd) == WideCharUpper(*wpOptionCount)));
               #else
                 (bSensitive == FALSE && (wchar_t)(WORD)(DWORD)CharUpperW((wchar_t *)(DWORD)(WORD)*wpLineEnd) == (wchar_t)(WORD)(DWORD)CharUpperW((wchar_t *)(DWORD)(WORD)*wpOptionCount)));
               #endif
             ++wpLineEnd)
        {
          if (!*++wpOptionCount)
          {
            wpLineStart=wpLineEnd;
            wpOptionString=wpLineEnd + 1;
            break;
          }
        }
      }
    }
    else if (wchQuote == '\0')
      wchQuote=*wpLineStart;
    else if (*wpLineStart == wchQuote)
      wchQuote='\0';
  }
  if (wpOptionString)
  {
    while (*--wpLineStart == ' ' || *wpLineStart == '\t');

    if (*wpOptionString == *wpLineStart && (*wpOptionString == '\"' || *wpOptionString == '\'' || *wpOptionString == '`'))
    {
      ++wpOptionString, --wpLineStart;
    }
    nBytes=wpLineStart - wpOptionString + 2;
    if (wszResult)
    {
      if (nMaxResult < nBytes) nBytes=nMaxResult;
      lstrcpynW(wszResult, wpOptionString, nBytes);
    }
  }
  return nBytes;
}
#endif

/********************************************************************
 *
 *  WordFindA
 *
 *String manipulation function.
 *
 * [in] const char *pText    Text.
 * [in] const char *pDelim   Delimiter.
 * [in] int nNumber          Number of the delimiter/word if positive
 *                            search from beginning if negative from end,
 *                            if (nNumber == 0) then returns sum of delimiters/words.
 * [in] const char *pOption  ">"     all text before founded delimiter.
 *                           "<"     all text after founded delimiter.
 *                           "<>"    deletes delimiter.
 *                           "*"     text between delimiters (word).
 *                           "*>"    all text after founded word.
 *                           ">*"    word and all text after founded word.
 *                           "<*"    all text before founded word.
 *                           "*<"    word and all text before founded word.
 *                           "<*>"   deletes word and neighbouring delimiter.
 * [in] BOOL bSensitive      TRUE   case sensitive.
 *                           FALSE  case insensitive.
 *[out] char *szResult       Output for result string, can be NULL.
 *[out] int *nMaxResult      Contains the length of the result string,
 *                            not including the terminating null character,
 *                            can be NULL.
 *[out] char **ppResult      Pointer to the first character of result string in pText,
 *                            can be NULL.
 *
 *Returns (nRes):  sum of delimiters/words, if (nNumber == 0)
 *                 -1 syntax error or pDelim is empty
 *                 -2 no delimiters found
 *                 -3 no such delimiter/word number
 *
 *Defines:
 * #define WordFindA_UNMINUS  //nNumber only positive (uses for minimize program size)
 * #define WordFindA_UNPLUS   //nNumber only negative (uses for minimize program size)
 ********************************************************************/
#if defined WordFindA || defined ALLSTRFUNC
#define WordFindA_INCLUDED
#undef WordFindA
int WordFindA(const char *pText, const char *pDelim, int nNumber, const char *pOption, BOOL bSensitive, char *szResult, int *nMaxResult, char **ppResult)
{
  const char *pTextStart;
  const char *pTextEnd;
  const char *pTextMinus=NULL;
  const char *pDelimCount;
  const char *pDelimMinus=NULL;
  const char *pWord;
  const char *pResult=NULL;
  BOOL bFound=FALSE;
  BOOL bRow=TRUE;
  int nOption=0;
  int nCurStrNumber=0;
  int nCurWordNumber=0;
  int nDelimLen=lstrlenA(pDelim);
  int nResultSize=0;

  if (!lstrcmpA(pOption, ">")) nOption=1;
  else if (!lstrcmpA(pOption, "<")) nOption=2;
  else if (!lstrcmpA(pOption, "<>")) nOption=3;
  else if (!lstrcmpA(pOption, "*")) nOption=4;
  else if (!lstrcmpA(pOption, "*>")) nOption=5;
  else if (!lstrcmpA(pOption, ">*")) nOption=6;
  else if (!lstrcmpA(pOption, "<*")) nOption=7;
  else if (!lstrcmpA(pOption, "*<")) nOption=8;
  else if (!lstrcmpA(pOption, "<*>")) nOption=9;

  if (nDelimLen == 0 || nOption == 0)
    return -1;

  #ifndef WordFindA_UNPLUS
  if (nNumber >= 0)
  {
    for (pWord=pTextStart=pTextEnd=pText; *pTextStart && *pTextEnd; ++pTextStart)
    {
      for (pTextEnd=pTextStart, pDelimCount=pDelim;
            *pTextEnd &&
            (*pTextEnd == *pDelimCount ||
            (bSensitive == FALSE && (char)(WORD)(DWORD)CharUpperA((char *)(DWORD)(WORD)*pTextEnd) == (char)(WORD)(DWORD)CharUpperA((char *)(DWORD)(WORD)*pDelimCount)));
           ++pTextEnd)
      {
        if (!*++pDelimCount)
        {
          if (nOption > 3)
          {
            if (bRow == FALSE && ++nCurWordNumber == nNumber)
            {
              if (nOption == 4)
              {
                pResult=pWord;
                nResultSize=pTextEnd - pResult - nDelimLen + 1;
              }
              else if (nOption == 5)
              {
                pResult=pTextEnd - nDelimLen + 1;
                nResultSize=lstrlenA(pResult);
              }
              else if (nOption == 6)
              {
                pResult=pWord;
                nResultSize=lstrlenA(pResult);
              }
              else if (nOption == 7)
              {
                pResult=pText;
                nResultSize=pWord - pResult;
              }
              else if (nOption == 8)
              {
                pResult=pText;
                nResultSize=pTextEnd - pResult - nDelimLen + 1;
              }
              else if (nOption == 9)
              {
                pResult=pTextEnd + 1;
                nResultSize=(pWord - pResult) + lstrlenA(pResult);
              }
              if (ppResult) *ppResult=(char *)pResult;
              if (nMaxResult) *nMaxResult=nResultSize;
              if (szResult)
              {
                if (nOption == 9)
                {
                  lstrcpynA(szResult, pText, pWord - pText + 1);
                  lstrcatA(szResult, pResult);
                }
                else lstrcpynA(szResult, pResult, nResultSize + 1);
              }
              return 0;
            }
            pWord=pTextEnd + 1;
            bRow=TRUE;
          }
          else if (++nCurStrNumber == nNumber)
          {
            if (nOption == 1)
            {
              pResult=pTextEnd + 1;
              nResultSize=lstrlenA(pResult);
            }
            else if (nOption == 2)
            {
              pResult=pText;
              nResultSize=pTextEnd - pResult - nDelimLen + 1;
            }
            else if (nOption == 3)
            {
              pResult=pTextEnd + 1;
              nResultSize=(pResult - pText - nDelimLen) + lstrlenA(pResult);
            }
            if (ppResult) *ppResult=(char *)pResult;
            if (nMaxResult) *nMaxResult=nResultSize;
            if (szResult)
            {
              if (nOption == 3)
              {
                lstrcpynA(szResult, pText, pTextEnd - pText - nDelimLen + 2);
                lstrcatA(szResult, pResult);
              }
              else lstrcpynA(szResult, pResult, nResultSize + 1);
            }
            return 0;
          }
          pTextStart=pTextEnd + 1;
          pDelimCount=pDelim;
          bFound=TRUE;
        }
      }
      if (*pTextStart && *pTextEnd) bRow=FALSE;
    }
    if (*pTextEnd) pTextEnd=pTextStart;

    if (nOption > 3 && bFound == TRUE && bRow == FALSE && ++nCurWordNumber == nNumber)
    {
      if (nOption == 4 || nOption == 6)
      {
        pResult=pWord;
        nResultSize=pTextEnd - pResult;
      }
      else if (nOption == 5)
      {
        pResult=pTextEnd;
        nResultSize=0;
      }
      else if (nOption == 7)
      {
        pResult=pText;
        nResultSize=pWord - pResult;
      }
      else if (nOption == 8)
      {
        pResult=pText;
        nResultSize=pTextEnd - pResult;
      }
      else if (nOption == 9)
      {
        pResult=pText;
        nResultSize=(pWord - nDelimLen) - pResult;
      }
      if (ppResult) *ppResult=(char *)pResult;
      if (nMaxResult) *nMaxResult=nResultSize;
      if (szResult)
      {
        lstrcpynA(szResult, pResult, nResultSize + 1);
      }
      return 0;
    }
    if (nNumber == 0)
    {
      if (nOption <= 3)
        return nCurStrNumber;
      return nCurWordNumber;
    }
  }
  #endif

  #ifndef WordFindA_UNMINUS
  else if (nNumber < 0)
  {
    pTextMinus=pTextStart=pTextEnd=pText + lstrlenA(pText) - 1;
    pDelimMinus=pDelim + nDelimLen - 1;
    pWord=pTextMinus + 1;

    for (; pTextEnd >= pText && pTextStart >= pText; --pTextEnd)
    {
      for (pTextStart=pTextEnd, pDelimCount=pDelimMinus;
            pTextStart >= pText &&
            (*pTextStart == *pDelimCount ||
            (bSensitive == FALSE && (char)(WORD)(DWORD)CharUpperA((char *)(DWORD)(WORD)*pTextStart) == (char)(WORD)(DWORD)CharUpperA((char *)(DWORD)(WORD)*pDelimCount)));
           --pTextStart)
      {
        if (--pDelimCount < pDelim)
        {
          if (nOption > 3)
          {
            if (bRow == FALSE && --nCurWordNumber == nNumber)
            {
              if (nOption == 4)
              {
                pResult=pTextStart + nDelimLen;
                nResultSize=pWord - pResult;
              }
              else if (nOption == 5)
              {
                pResult=pWord;
                nResultSize=pTextMinus - pResult + 1;
              }
              else if (nOption == 6)
              {
                pResult=pTextStart + nDelimLen;
                nResultSize=pTextMinus - pResult + 1;
              }
              else if (nOption == 7)
              {
                pResult=pText;
                nResultSize=(pTextStart + nDelimLen) - pResult;
              }
              else if (nOption == 8)
              {
                pResult=pText;
                nResultSize=pWord - pResult;
              }
              else if (nOption == 9)
              {
                pResult=pTextStart;
                nResultSize=(pResult - pText) + (pTextMinus - pWord) + 1;
              }
              if (ppResult) *ppResult=(char *)pResult;
              if (nMaxResult) *nMaxResult=nResultSize;
              if (szResult)
              {
                if (nOption == 9)
                {
                  lstrcpynA(szResult, pText, pTextStart - pText + 1);
                  lstrcatA(szResult, pWord);
                }
                else lstrcpynA(szResult, pResult, nResultSize + 1);
              }
              return 0;
            }
            pWord=pTextStart;
            bRow=TRUE;
          }
          else if (--nCurStrNumber == nNumber)
          {
            if (nOption == 1)
            {
              pResult=pTextStart + nDelimLen;
              nResultSize=pTextMinus - pResult + 1;
            }
            else if (nOption == 2)
            {
              pResult=pText;
              nResultSize=pTextStart - pResult;
            }
            else if (nOption == 3)
            {
              pResult=pTextStart;
              nResultSize=pTextMinus - pText - nDelimLen + 1;
            }
            if (ppResult) *ppResult=(char *)pResult;
            if (nMaxResult) *nMaxResult=nResultSize;
            if (szResult)
            {
              if (nOption == 3)
              {
                lstrcpynA(szResult, pText, pTextStart - pText + 1);
                lstrcatA(szResult, pTextStart + nDelimLen);
              }
              else lstrcpynA(szResult, pResult, nResultSize + 1);
            }
            return 0;
          }
          pTextEnd=pTextStart;
          pDelimCount=pDelimMinus;
          bFound=TRUE;
        }
      }
      if (pTextEnd >= pText && pTextStart >= pText) bRow=FALSE;
    }
    if (nOption > 3 && bFound == TRUE && bRow == FALSE && --nCurWordNumber == nNumber)
    {
      if (nOption == 4 || nOption == 8)
      {
        pResult=pText;
        nResultSize=pWord - pResult;
      }
      else if (nOption == 5)
      {
        pResult=pWord;
        nResultSize=pTextMinus - pResult + 1;
      }
      else if (nOption == 6)
      {
        pResult=pText;
        nResultSize=pTextMinus - pResult + 1;
      }
      else if (nOption == 7)
      {
        pResult=pText;
        nResultSize=0;
      }
      else if (nOption == 9)
      {
        pResult=pWord + nDelimLen;
        nResultSize=pTextMinus - pResult + 1;
      }
      if (ppResult) *ppResult=(char *)pResult;
      if (nMaxResult) *nMaxResult=nResultSize;
      if (szResult)
      {
        lstrcpynA(szResult, pResult, nResultSize + 1);
      }
      return 0;
    }
  }
  #endif

  if (bFound == TRUE)
    return -3;
  else
    return -2;
}
#endif

/********************************************************************
 *
 *  WordFindW
 *
 *Unicode string manipulation function.
 *
 * [in] const wchar_t *wpText    Text.
 * [in] const wchar_t *wpDelim   Delimiter.
 * [in] int nNumber              Number of the delimiter/word if positive
 *                                search from beginning if negative from end,
 *                                if (nNumber == 0) then returns sum of delimiters/words.
 * [in] const wchar_t *wpOption  L">"     all text before founded delimiter.
 *                               L"<"     all text after founded delimiter.
 *                               L"<>"    deletes delimiter.
 *                               L"*"     text between delimiters (word).
 *                               L"*>"    all text after founded word.
 *                               L">*"    word and all text after founded word.
 *                               L"<*"    all text before founded word.
 *                               L"*<"    word and all text before founded word.
 *                               L"<*>"   deletes word and neighbouring delimiter.
 * [in] BOOL bSensitive          TRUE   case sensitive.
 *                               FALSE  case insensitive.
 *[out] wchar_t *wszResult       Output for result string, can be NULL.
 *[out] int *nMaxResult          Contains the length of the result string,
 *                                not including the terminating null character,
 *                                can be NULL.
 *[out] wchar_t **wppResult      Pointer to the first character of result string in wpText,
 *                                can be NULL.
 *
 *Returns (nRes):  sum of delimiters/words, if (nNumber == 0)
 *                 -1 syntax error or wpDelim is empty
 *                 -2 no delimiters found
 *                 -3 no such delimiter/word number
 *
 *Defines:
 * #define WordFindW_UNMINUS  //nNumber only positive (uses for minimize program size)
 * #define WordFindW_UNPLUS   //nNumber only negative (uses for minimize program size)
 ********************************************************************/
#if defined WordFindW || defined ALLSTRFUNC
#define WordFindW_INCLUDED
#undef WordFindW
int WordFindW(const wchar_t *wpText, const wchar_t *wpDelim, int nNumber, const wchar_t *wpOption, BOOL bSensitive, wchar_t *wszResult, int *nMaxResult, wchar_t **wppResult)
{
  const wchar_t *wpTextStart;
  const wchar_t *wpTextEnd;
  const wchar_t *wpTextMinus=NULL;
  const wchar_t *wpDelimCount;
  const wchar_t *wpDelimMinus=NULL;
  const wchar_t *wpWord;
  const wchar_t *wpResult=NULL;
  BOOL bFound=FALSE;
  BOOL bRow=TRUE;
  int nOption=0;
  int nCurStrNumber=0;
  int nCurWordNumber=0;
  int nDelimLen=lstrlenW(wpDelim);
  int nResultSize=0;

  if (!lstrcmpW(wpOption, L">")) nOption=1;
  else if (!lstrcmpW(wpOption, L"<")) nOption=2;
  else if (!lstrcmpW(wpOption, L"<>")) nOption=3;
  else if (!lstrcmpW(wpOption, L"*")) nOption=4;
  else if (!lstrcmpW(wpOption, L"*>")) nOption=5;
  else if (!lstrcmpW(wpOption, L">*")) nOption=6;
  else if (!lstrcmpW(wpOption, L"<*")) nOption=7;
  else if (!lstrcmpW(wpOption, L"*<")) nOption=8;
  else if (!lstrcmpW(wpOption, L"<*>")) nOption=9;

  if (nDelimLen == 0 || nOption == 0)
    return -1;

  #ifndef WordFindW_UNPLUS
  if (nNumber >= 0)
  {
    for (wpWord=wpTextStart=wpTextEnd=wpText; *wpTextStart && *wpTextEnd; ++wpTextStart)
    {
      for (wpTextEnd=wpTextStart, wpDelimCount=wpDelim;
            *wpTextEnd &&
            (*wpTextEnd == *wpDelimCount ||
            (bSensitive == FALSE && (wchar_t)(WORD)(DWORD)CharUpperW((wchar_t *)(DWORD)(WORD)*wpTextEnd) == (wchar_t)(WORD)(DWORD)CharUpperW((wchar_t *)(DWORD)(WORD)*wpDelimCount)));
           ++wpTextEnd)
      {
        if (!*++wpDelimCount)
        {
          if (nOption > 3)
          {
            if (bRow == FALSE && ++nCurWordNumber == nNumber)
            {
              if (nOption == 4)
              {
                wpResult=wpWord;
                nResultSize=wpTextEnd - wpResult - nDelimLen + 1;
              }
              else if (nOption == 5)
              {
                wpResult=wpTextEnd - nDelimLen + 1;
                nResultSize=lstrlenW(wpResult);
              }
              else if (nOption == 6)
              {
                wpResult=wpWord;
                nResultSize=lstrlenW(wpResult);
              }
              else if (nOption == 7)
              {
                wpResult=wpText;
                nResultSize=wpWord - wpResult;
              }
              else if (nOption == 8)
              {
                wpResult=wpText;
                nResultSize=wpTextEnd - wpResult - nDelimLen + 1;
              }
              else if (nOption == 9)
              {
                wpResult=wpTextEnd + 1;
                nResultSize=(wpWord - wpResult) + lstrlenW(wpResult);
              }
              if (wppResult) *wppResult=(wchar_t *)wpResult;
              if (nMaxResult) *nMaxResult=nResultSize;
              if (wszResult)
              {
                if (nOption == 9)
                {
                  lstrcpynW(wszResult, wpText, wpWord - wpText + 1);
                  lstrcatW(wszResult, wpResult);
                }
                else lstrcpynW(wszResult, wpResult, nResultSize + 1);
              }
              return 0;
            }
            wpWord=wpTextEnd + 1;
            bRow=TRUE;
          }
          else if (++nCurStrNumber == nNumber)
          {
            if (nOption == 1)
            {
              wpResult=wpTextEnd + 1;
              nResultSize=lstrlenW(wpResult);
            }
            else if (nOption == 2)
            {
              wpResult=wpText;
              nResultSize=wpTextEnd - wpResult - nDelimLen + 1;
            }
            else if (nOption == 3)
            {
              wpResult=wpTextEnd + 1;
              nResultSize=(wpResult - wpText - nDelimLen) + lstrlenW(wpResult);
            }
            if (wppResult) *wppResult=(wchar_t *)wpResult;
            if (nMaxResult) *nMaxResult=nResultSize;
            if (wszResult)
            {
              if (nOption == 3)
              {
                lstrcpynW(wszResult, wpText, wpTextEnd - wpText - nDelimLen + 2);
                lstrcatW(wszResult, wpResult);
              }
              else lstrcpynW(wszResult, wpResult, nResultSize + 1);
            }
            return 0;
          }
          wpTextStart=wpTextEnd + 1;
          wpDelimCount=wpDelim;
          bFound=TRUE;
        }
      }
      if (*wpTextStart && *wpTextEnd) bRow=FALSE;
    }
    if (*wpTextEnd) wpTextEnd=wpTextStart;

    if (nOption > 3 && bFound == TRUE && bRow == FALSE && ++nCurWordNumber == nNumber)
    {
      if (nOption == 4 || nOption == 6)
      {
        wpResult=wpWord;
        nResultSize=wpTextEnd - wpResult;
      }
      else if (nOption == 5)
      {
        wpResult=wpTextEnd;
        nResultSize=0;
      }
      else if (nOption == 7)
      {
        wpResult=wpText;
        nResultSize=wpWord - wpResult;
      }
      else if (nOption == 8)
      {
        wpResult=wpText;
        nResultSize=wpTextEnd - wpResult;
      }
      else if (nOption == 9)
      {
        wpResult=wpText;
        nResultSize=(wpWord - nDelimLen) - wpResult;
      }
      if (wppResult) *wppResult=(wchar_t *)wpResult;
      if (nMaxResult) *nMaxResult=nResultSize;
      if (wszResult)
      {
        lstrcpynW(wszResult, wpResult, nResultSize + 1);
      }
      return 0;
    }
    if (nNumber == 0)
    {
      if (nOption <= 3)
        return nCurStrNumber;
      return nCurWordNumber;
    }
  }
  #endif

  #ifndef WordFindW_UNMINUS
  else if (nNumber < 0)
  {
    wpTextMinus=wpTextStart=wpTextEnd=wpText + lstrlenW(wpText) - 1;
    wpDelimMinus=wpDelim + nDelimLen - 1;
    wpWord=wpTextMinus + 1;

    for (; wpTextEnd >= wpText && wpTextStart >= wpText; --wpTextEnd)
    {
      for (wpTextStart=wpTextEnd, wpDelimCount=wpDelimMinus;
            wpTextStart >= wpText &&
            (*wpTextStart == *wpDelimCount ||
            (bSensitive == FALSE && (wchar_t)(WORD)(DWORD)CharUpperW((wchar_t *)(DWORD)(WORD)*wpTextStart) == (wchar_t)(WORD)(DWORD)CharUpperW((wchar_t *)(DWORD)(WORD)*wpDelimCount)));
           --wpTextStart)
      {
        if (--wpDelimCount < wpDelim)
        {
          if (nOption > 3)
          {
            if (bRow == FALSE && --nCurWordNumber == nNumber)
            {
              if (nOption == 4)
              {
                wpResult=wpTextStart + nDelimLen;
                nResultSize=wpWord - wpResult;
              }
              else if (nOption == 5)
              {
                wpResult=wpWord;
                nResultSize=wpTextMinus - wpResult + 1;
              }
              else if (nOption == 6)
              {
                wpResult=wpTextStart + nDelimLen;
                nResultSize=wpTextMinus - wpResult + 1;
              }
              else if (nOption == 7)
              {
                wpResult=wpText;
                nResultSize=(wpTextStart + nDelimLen) - wpResult;
              }
              else if (nOption == 8)
              {
                wpResult=wpText;
                nResultSize=wpWord - wpResult;
              }
              else if (nOption == 9)
              {
                wpResult=wpTextStart;
                nResultSize=(wpResult - wpText) + (wpTextMinus - wpWord) + 1;
              }
              if (wppResult) *wppResult=(wchar_t *)wpResult;
              if (nMaxResult) *nMaxResult=nResultSize;
              if (wszResult)
              {
                if (nOption == 9)
                {
                  lstrcpynW(wszResult, wpText, wpTextStart - wpText + 1);
                  lstrcatW(wszResult, wpWord);
                }
                else lstrcpynW(wszResult, wpResult, nResultSize + 1);
              }
              return 0;
            }
            wpWord=wpTextStart;
            bRow=TRUE;
          }
          else if (--nCurStrNumber == nNumber)
          {
            if (nOption == 1)
            {
              wpResult=wpTextStart + nDelimLen;
              nResultSize=wpTextMinus - wpResult + 1;
            }
            else if (nOption == 2)
            {
              wpResult=wpText;
              nResultSize=wpTextStart - wpResult;
            }
            else if (nOption == 3)
            {
              wpResult=wpTextStart;
              nResultSize=wpTextMinus - wpText - nDelimLen + 1;
            }
            if (wppResult) *wppResult=(wchar_t *)wpResult;
            if (nMaxResult) *nMaxResult=nResultSize;
            if (wszResult)
            {
              if (nOption == 3)
              {
                lstrcpynW(wszResult, wpText, wpTextStart - wpText + 1);
                lstrcatW(wszResult, wpTextStart + nDelimLen);
              }
              else lstrcpynW(wszResult, wpResult, nResultSize + 1);
            }
            return 0;
          }
          wpTextEnd=wpTextStart;
          wpDelimCount=wpDelimMinus;
          bFound=TRUE;
        }
      }
      if (wpTextEnd >= wpText && wpTextStart >= wpText) bRow=FALSE;
    }
    if (nOption > 3 && bFound == TRUE && bRow == FALSE && --nCurWordNumber == nNumber)
    {
      if (nOption == 4 || nOption == 8)
      {
        wpResult=wpText;
        nResultSize=wpWord - wpResult;
      }
      else if (nOption == 5)
      {
        wpResult=wpWord;
        nResultSize=wpTextMinus - wpResult + 1;
      }
      else if (nOption == 6)
      {
        wpResult=wpText;
        nResultSize=wpTextMinus - wpResult + 1;
      }
      else if (nOption == 7)
      {
        wpResult=wpText;
        nResultSize=0;
      }
      else if (nOption == 9)
      {
        wpResult=wpWord + nDelimLen;
        nResultSize=wpTextMinus - wpResult + 1;
      }
      if (wppResult) *wppResult=(wchar_t *)wpResult;
      if (nMaxResult) *nMaxResult=nResultSize;
      if (wszResult)
      {
        lstrcpynW(wszResult, wpResult, nResultSize + 1);
      }
      return 0;
    }
  }
  #endif

  if (bFound == TRUE)
    return -3;
  else
    return -2;
}
#endif


/********************************************************************
 *                                                                  *
 *                           Example                                *
 *                                                                  *
 ********************************************************************

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include "StrFunc.h"

//insert functions
#define WordFindA
#define StrReplaceA
#define GetOptionsA
#define xstrstrA
#define xstrcmpiA
#define xstrcmpinA
#include "StrFunc.h"

void main()
{
  char szResult[MAX_PATH];
  char *pStringBegin=NULL;
  char *pStringEnd=NULL;
  int nStringLen;
  int nError;

  nError=WordFindA("ABC||dfg||HJK", "||", 2, "*", TRUE, szResult, &nStringLen, &pStringBegin);
  printf("szResult={%s}, nStringLen={%d}, pStringBegin={%s}, nError={%d}\n", szResult, nStringLen, pStringBegin, nError);

  nError=StrReplaceA("ABC||dfg||HJK", "||", "##", TRUE, szResult, &nStringLen);
  printf("szResult={%s}, nStringLen={%d}, nError={%d}\n", szResult, nStringLen, nError);

  nError=GetOptionsA("/A=123 /B=\"456\" /C=`789`", "/b=", FALSE, szResult, MAX_PATH);
  printf("szResult={%s}, nError={%d}\n", szResult, nError);

  nError=xstrstrA("ABC||dfg||HJK", (DWORD)-1, "Dfg", FALSE, &pStringBegin, &pStringEnd);
  printf("pStringBegin={%s}, pStringEnd={%s}, nError={%d}\n", pStringBegin, pStringEnd, nError);

  nError=xstrcmpiA("ABC", "abc");
  printf("nError={%d}\n", nError);

  nError=xstrcmpinA("ABCdfg", "abcxyz", 3);
  printf("nError={%d}\n", nError);
}

*/