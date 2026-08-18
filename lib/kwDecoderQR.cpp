#include "kwDecoderQR.h"

//-------------------------------------
void kwDecoderQR::UnMask(kwImageU8 source, int maskNum, int verNum, kwImageU8 &unmasked_result)
{
    switch( maskNum )
    {
    case 0:
        for (int i = 0; i < source.rows; ++i)
            for (int j = 0; j < source.cols; ++j)
                source.imgElement[i][j] = ( (i+j)%2 == 0 ) ? source.imgElement[i][j]^0x0001 : source.imgElement[i][j];
        break;
    case 1:
        for (int i = 0; i < source.rows; ++i)
            for (int j = 0; j < source.cols; ++j)
                source.imgElement[i][j] = ( i%2 == 0 ) ? source.imgElement[i][j]^0x0001 : source.imgElement[i][j];
        break;
    case 2:
        for (int i = 0; i < source.rows; ++i)
            for (int j = 0; j < source.cols; ++j)
                source.imgElement[i][j] = ( j%3 == 0 ) ? source.imgElement[i][j]^0x0001 : source.imgElement[i][j];
        break;
    case 3:
        for (int i = 0; i < source.rows; ++i)
            for (int j = 0; j < source.cols; ++j)
                source.imgElement[i][j] = ( (i+j)%3 == 0 ) ? source.imgElement[i][j]^0x0001 : source.imgElement[i][j];
        break;
    case 4:
        for (int i = 0; i < source.rows; ++i)
            for (int j = 0; j < source.cols; ++j)
                source.imgElement[i][j] = ( ((int)i/2+(int)j/3)%2 == 0 ) ? source.imgElement[i][j]^0x0001 : source.imgElement[i][j];
        break;
    case 5:
        for (int i = 0; i < source.rows; ++i)
            for (int j = 0; j < source.cols; ++j)
                source.imgElement[i][j] = ( ((i*j)%2 + (i*j)%3) == 0 ) ? source.imgElement[i][j]^0x0001 : source.imgElement[i][j];
        break;
    case 6:
        for (int i = 0; i < source.rows; ++i)
            for (int j = 0; j < source.cols; ++j)
                source.imgElement[i][j] = ( ((i*j)%2 + (i*j)%3)%2 == 0 ) ? source.imgElement[i][j]^0x0001 : source.imgElement[i][j];
        break;
    case 7:
        for (int i = 0; i < source.rows; ++i)
            for (int j = 0; j < source.cols; ++j)
                source.imgElement[i][j] = ( ((i+j)%2 + (i*j)%3)%2 == 0 ) ? source.imgElement[i][j]^0x0001 : source.imgElement[i][j];
        break;
    }

    // Mark the function pattern
    int qrsize = 7*2 + 2 + 1 + verNum * 4;

    //  左上
    for (int i = 0; i < 9; ++i)
        for (int j = 0; j < 9; ++j)
            source.imgElement[i][j] = 'F';
    // 右上
    for (int i = 0; i < 9; ++i)
        for (int j = 0; j < 8; ++j)
          source.imgElement[i][qrsize-1 - j] = 'F';
    // 左下
    for (int i = 0; i < 8; ++i)
        for (int j = 0; j < 9; ++j)
          source.imgElement[qrsize-1 - i][j] = 'F';

    // timing line
    for (int j = 0; j < qrsize; ++j)
        source.imgElement[6][j] = 'F';
    for (int i = 0; i < qrsize; ++i)
        source.imgElement[i][6] = 'F';

    // Alignment
    if (verNum == 2 || verNum == 3 || verNum == 4 || verNum == 5 || verNum == 6) {
        for (int i = 0; i < 5; ++i)
          for (int j = 0; j < 5; ++j)
            source.imgElement[qrsize-1 - 4 - i][qrsize-1 - 4 - j] = 'F';
    }
    unmasked_result = source;
    return;
}

//-------------------------------------
void kwDecoderQR::GetCodeword(kwImageU8 source, int verNum, int ECClevel, kwPixel codeWordBefore[])
{
    enum direction { UP = -1, DOWN = 1, LEFT = 2, RIGHT = 3 };
    unsigned short int decodedCode;     // 0 ~ 65,535 (16 bits), 可以用 unsigned char 嗎?
    int numofTotalWords, virticalNow, nextLR;
    kwPointInt passed(source.rows - 1, source.cols - 1);

    int index = GetIndex( verNum, ECClevel );
    numofTotalWords = Ecc_S_c[index]*Ecc_S_numofBlocks[index] + Ecc_L_k[index]*Ecc_L_numofBlocks[index];

    virticalNow = UP;
    nextLR = LEFT;

    // retrival
    for (int t = 0; t < numofTotalWords; ++t)
    {
        decodedCode = 0;
        for (int i = 0; i < 8; ++i) {
            // 跳過不能走的
            while ( source.imgElement[passed.y][passed.x] == 'P' || source.imgElement[(int)passed.y][(int)passed.x] == 'F' )
            {
                if (passed.x == 6)  // special pattern
                    passed.x = passed.x - 1;
                if( nextLR == LEFT ){
                    passed.x -= 1;
                    nextLR = RIGHT;
                }
                else if ( nextLR == RIGHT ){
                    if ( passed.y == 0 && virticalNow == UP ){   // i.e. passed.y + virticalNow < 0
                        passed.x -= 1;
                        virticalNow = DOWN;
                    }
                    else if ( passed.y == source.rows-1 && virticalNow == DOWN ) {
                        passed.x -= 1;
                        virticalNow = UP;
                    }
                    else {
                        passed.x += 1;
                        passed.y += virticalNow;
                    }
                    nextLR = LEFT;
                }
            }
            // 取出 information
            if (source.imgElement[passed.y][passed.x] == 0) // 0 means black in kwImageU8
                decodedCode += 1;   // 條碼的 black 是代表要 +1
            if (i != 7)
                decodedCode <<= 1;
            source.imgElement[passed.y][passed.x] = 'P';
        }
        codeWordBefore[t] = (kwPixel)decodedCode;
    }
    return;
}

//-------------------------------------
void kwDecoderQR::Solve_Interleaving(int verNum, int ECClevel, kwPixel codeWordBefore[], kwPixel* *saperatedData)
{
    return;
}

//-------------------------------------
int kwDecoderQR::GetDecodeData(int verNum, int ECClevel, kwPixel decodedDataBefore[], kwPixel decodedDataAfter[])
{
    CxDecoderGRS_QR RScode;
    RScode.SetInputOutput(decodedDataBefore, decodedDataAfter);
    int index = GetIndex( verNum, ECClevel );
    return RScode.Decode(Ecc_S_c[index], Ecc_S_k[index]+Ecc_p[index]);
}

//-------------------------------------
int kwDecoderQR::GetBitSequence(unsigned cBytePool[], int nNumBytes, int iStartBit, int nNumBitsToGet)
{ // at most 16 bits
    unsigned longword;                // 32-bit long word
    unsigned beginbitloc, endbitloc;  // starting/ending bit locations in the longword

    if ((nNumBitsToGet>16)||((iStartBit+nNumBitsToGet-1)>nNumBytes*8-1))
        return -1;
    longword = (cBytePool[iStartBit/8]<<16) + (cBytePool[iStartBit/8+1]<<8) + cBytePool[iStartBit/8+2];
    beginbitloc = 7-iStartBit%8+16;
    endbitloc = beginbitloc-nNumBitsToGet+1;

    longword >>= endbitloc;

    int p2n=1;
    p2n <<= nNumBitsToGet;
    return( longword % p2n );
}

//-------------------------------------
int kwDecoderQR::UncompressText(unsigned char cByteString[], int nNumBytes, int nSymVersion, unsigned char cOutputText[])
{
int      nDataLen;
int      DataCharCountNBits, ThreeDigitData, TwoLetterData;
int      iChar, iCharBase=0;
int      mode, data;
unsigned iNextBitRetrieve = 0;

// conversion to unsigned int
unsigned* iCodewordStr = new unsigned[nNumBytes];
for (int ii=0; ii<nNumBytes; ++ii)
    iCodewordStr[ii] = (unsigned)cByteString[ii];

// start decoding
while ( iNextBitRetrieve < (unsigned)(nNumBytes*8-4) )
   {
   if ( (mode = GetBitSequence(iCodewordStr, nNumBytes, iNextBitRetrieve, 4)) < 0 )   // retrieve the first 4 bits as the Data Mode.
   {
       delete [] iCodewordStr;
       return 0;
   }
   iNextBitRetrieve+=4;

   if (mode==3) // Structured Append
       continue;

   if (mode==0) // terminator
      break;

   if (mode==5) // FNC1
       continue;

   // check if it is in ECI mode
   if (mode==7)
       continue;

   if (nSymVersion<=9)
       switch (mode)
       {
       case 1: DataCharCountNBits = 10; break;
       case 2: DataCharCountNBits = 9; break;
       case 4: DataCharCountNBits = 8; break;
       case 8: DataCharCountNBits = 8; break;
       default: DataCharCountNBits = 0; break;
       }
   else
   {
       if ((nSymVersion>=10)&&(nSymVersion<=26))
       {
           switch (mode)
           {
           case 1: DataCharCountNBits = 12; break;
           case 2: DataCharCountNBits = 11; break;
           case 4: DataCharCountNBits = 16; break;
           case 8: DataCharCountNBits = 10; break;
           case 7: DataCharCountNBits = 8; break; // ECI (in a byte)
           default: DataCharCountNBits = 0; break;
           };
       }
       else
       {
           switch (mode)
           {
           case 1: DataCharCountNBits = 14; break;
           case 2: DataCharCountNBits = 13; break;
           case 4: DataCharCountNBits = 16; break;
           case 8: DataCharCountNBits = 12; break;
           case 7: DataCharCountNBits = 8; break; // ECI (in a byte)
           default: DataCharCountNBits = 0; break;
           };
       }
   }

   if (DataCharCountNBits==0)
   {
       delete [] iCodewordStr;
       return 0;
   }

   if ( ( nDataLen = GetBitSequence(iCodewordStr, nNumBytes, iNextBitRetrieve, DataCharCountNBits) ) < 0 )
   {
       delete [] iCodewordStr;
       return 0;
   }

   iNextBitRetrieve += DataCharCountNBits;
   if (nDataLen==0)
   {
       delete [] iCodewordStr;
       return 0;
   }

   switch(mode)
      {
      case 1: // Mode = Numeric           0001
              for (iChar=0; iChar+3<=(nDataLen); iChar+=3, iNextBitRetrieve+=10)
                  {
                  if ( (data=GetBitSequence(iCodewordStr, nNumBytes, iNextBitRetrieve, 10))<0 )
                  {
                      delete [] iCodewordStr;
                      return 0;
                  }
                  ThreeDigitData = data;
                  cOutputText[iCharBase+iChar]   = ThreeDigitData/100+'0';
                  cOutputText[iCharBase+iChar+1] = (ThreeDigitData%100)/10+'0';
                  cOutputText[iCharBase+iChar+2] = ThreeDigitData%10+'0';
                  }
              switch((nDataLen)-iChar)
                 {
                 case 0: // no digit left
                         cOutputText[iCharBase+iChar]   = 0;
                         break;
                 case 1: // 'one' more digit left
                         if ( (data=GetBitSequence(iCodewordStr, nNumBytes, iNextBitRetrieve, 4))<0 )
                         {
                             delete [] iCodewordStr;
                             return 0;
                         }
                         ThreeDigitData = data;
                         cOutputText[iCharBase+iChar]     = ThreeDigitData+'0';
                         cOutputText[iCharBase+iChar+1]   = 0;
                         iNextBitRetrieve += 4;
                         break;
                 case 2: // 'two' more digits left
                         if ( (data=GetBitSequence(iCodewordStr, nNumBytes, iNextBitRetrieve, 7))<0 )
                         {
                             delete [] iCodewordStr;
                             return 0;
                         }
                         ThreeDigitData = data;
                         cOutputText[iCharBase+iChar]   = ThreeDigitData/10+'0';
                         cOutputText[iCharBase+iChar+1] = ThreeDigitData%10+'0';
                         cOutputText[iCharBase+iChar+2] = 0;
                         iNextBitRetrieve += 7;
                         break;
                 }
              iCharBase+=(nDataLen);
              break;

      case 2: // Mode = Alphanumeric      0010
              for (iChar=0; iChar+2<=(nDataLen); iChar+=2, iNextBitRetrieve+=11)
                  {
                  if ( (data=GetBitSequence(iCodewordStr, nNumBytes, iNextBitRetrieve, 11))<0 )
                  {
                      delete [] iCodewordStr;
                      return 0;
                  }
                  TwoLetterData = data;
                  cOutputText[iCharBase+iChar]   = AlphaNumeric_Table[TwoLetterData/45];
                  cOutputText[iCharBase+iChar+1] = AlphaNumeric_Table[TwoLetterData%45];
                  }
              switch((nDataLen)-iChar)
                 {
                 case 0: // no letter left
                         cOutputText[iCharBase+iChar]   = 0;
                         break;
                 case 1: // 'one' more letter left
                         if ( (data=GetBitSequence(iCodewordStr, nNumBytes, iNextBitRetrieve, 6))<0 )
                         {
                             delete [] iCodewordStr;
                             return 0;
                         }
                         TwoLetterData = data;
                         cOutputText[iCharBase+iChar]     = AlphaNumeric_Table[TwoLetterData];
                         cOutputText[iCharBase+iChar+1]   = 0;
                         iNextBitRetrieve+=6;
                         break;
                 }
              iCharBase+=(nDataLen);
              break;

      case 4: // Mode = Byte              0100
              for (iChar=0; iChar < nDataLen; iChar++, iNextBitRetrieve+=8)
                  {
                  if ( (data = GetBitSequence(iCodewordStr, nNumBytes, iNextBitRetrieve, 8))<0 )
                  {
                      delete [] iCodewordStr;
                      return 0;
                  }
                  cOutputText[iCharBase+iChar]=(unsigned char)data;
                  }
              cOutputText[iCharBase+iChar]=0;
              iCharBase+=(nDataLen);
              break;

      case 8: // Mode = Kanji            1000
              unsigned Kanji16bits, Kanji13bits;
              for (iChar=0; iChar < nDataLen; iChar++, iNextBitRetrieve+=13)
                  {
                  if ( (data=GetBitSequence(iCodewordStr, nNumBytes, iNextBitRetrieve, 13))<0 )
                  {
                      delete [] iCodewordStr;
                      return 0;
                  }
                  Kanji13bits = data;
                  Kanji16bits = (Kanji13bits/0x000000C0)*0x00000100 + (Kanji13bits%0x000000C0) + ((Kanji13bits<0x1740) ? 0x00008140 : 0x0000C140);
                  cOutputText[iCharBase+iChar*2]   = ( Kanji16bits >> 8 );
                  cOutputText[iCharBase+iChar*2+1] = ( Kanji16bits & 0x00FF );
                  }
              cOutputText[ iCharBase = iCharBase + (nDataLen<<1) ] = 0;
              break;

      case 7: // Mode = ECI               0111
              break;

      default: // exception : wrong mode number
              {
                  delete [] iCodewordStr;
                  return 0;
              }
      }
   }

delete [] iCodewordStr;
return iCharBase;
}

//-------------------------------------
int kwDecoderQR::GetIndex(int verNum, int ECClevel)
{
    int Index = 4*(verNum-1);
    switch ( ECClevel ) {
        case 0:   //M
            return Index+1;
        case 1:   //L
            return Index;
        case 2:   //H
            return Index+3;
        case 3:   //Q
            return Index+2;
    }
}


//------------------------------------- Ver. Zx
//void kwDecoderQR::DecodeText(kwPixel decodeDataAfter[], kwPixel decodedText[], int verNum, int ECClevel)
//{
//    enum MODE { End = 0, Numeric = 1, AlphaNumeric = 2, bit_8 = 4 };
//    int DATA_Num, CountIndicator, Mode, DataRemainder, EncodingWord, EncodeNum;
//    unsigned char* DataPosition;
//    switch ( verNum )
//    {
//        case 1:
//            switch ( ECClevel ) {
//                case 1:
//                  DATA_Num = 19 * 8;
//                  break;
//                case 0:
//                  DATA_Num = 16 * 8;
//                  break;
//                case 3:
//                  DATA_Num = 13 * 8;
//                  break;
//                case 2:
//                  DATA_Num = 9 * 8;
//                  break;
//            }
//            break;
//        case 2:
//            switch ( ECClevel ) {
//                case 1:
//                  DATA_Num = 34 * 8;
//                  break;
//                case 0:
//                  DATA_Num = 28 * 8;
//                  break;
//                case 3:
//                  DATA_Num = 22 * 8;
//                  break;
//                case 2:
//                  DATA_Num = 16 * 8;
//                  break;
//            }
//            break;
//        case 3:
//            switch ( ECClevel ) {
//                case 1:
//                  DATA_Num = 55 * 8;
//                  break;
//                case 0:
//                  DATA_Num = 44 * 8;
//                  break;
//                case 3:
//                  DATA_Num = 17 * 8;
//                  break;
//                case 2:
//                  DATA_Num = 13 * 8;
//                  break;
//            }
//            break;
//        case 4:
//            switch ( ECClevel ) {
//                case 1:
//                  DATA_Num = 80 * 8;
//                  break;
//                case 0:
//                  DATA_Num = 32 * 8;
//                  break;
//                case 3:
//                  DATA_Num = 24 * 8;
//                  break;
//                case 2:
//                  DATA_Num = 9 * 8;
//                  break;
//            }
//            break;
//        case 5:
//            switch ( ECClevel ) {
//                case 1:
//                  DATA_Num = 108 * 8;
//                  break;
//                case 0:
//                  DATA_Num = 43 * 8;
//                  break;
//                case 3:
//                  DATA_Num = 15 * 8;
//                  break;
//                case 2:
//                  DATA_Num = 11 * 8;
//                  break;
//            }
//            break;
//        case 6:
//            switch (ECClevel) {
//                case 1:
//                  DATA_Num = 68 * 8;
//                  break;
//                case 0:
//                  DATA_Num = 27 * 8;
//                  break;
//                case 3:
//                  DATA_Num = 19 * 8;
//                  break;
//                case 2:
//                  DATA_Num = 15 * 8;
//                  break;
//            }
//            break;
//    }

//    DataPosition = decodeDataAfter;
//    DataRemainder = 8;
//    EncodeNum = 0;
//    while (DATA_Num > 0) {
//        // mode
//        Mode = Decoedinprocesss(4, DataPosition, DataRemainder);
//        DATA_Num -= 4;

//        switch (Mode) {
//          case End:
//            return;
//          case Numeric:

//            if (verNum == 1 || verNum == 2 || verNum == 3 || verNum == 4 || verNum == 5 ||
//                verNum == 6) {
//              CountIndicator = Decoedinprocesss(10, DataPosition, DataRemainder);
//              DATA_Num -= 10;
//              int CountRemainder = CountIndicator % 3;

//              CountIndicator /= 3;

//              while (CountIndicator > 0) {
//                EncodingWord = Decoedinprocesss(10, DataPosition, DataRemainder);

//                EncodeNum += 3;
//                for (int i = 0; i < 3; i++) {
//                  decodedText[EncodeNum - i - 1] = EncodingWord % 10 + 48;
//                  EncodingWord = EncodingWord / 10;
//                }
//                DATA_Num -= 10;
//                CountIndicator -= 1;
//              }
//              if (CountRemainder == 1) {
//                EncodingWord = Decoedinprocesss(4, DataPosition, DataRemainder);
//                decodedText[EncodeNum] = EncodingWord + 48;
//                DATA_Num -= 4;
//                EncodeNum++;
//              } else if (CountRemainder == 2) {
//                EncodingWord = Decoedinprocesss(7, DataPosition, DataRemainder);
//                EncodeNum += 2;
//                for (int i = 0; i < 2; i++) {
//                  decodedText[EncodeNum - i - 1] = EncodingWord % 10 + 48;
//                  EncodingWord = EncodingWord / 10;
//                }
//                DATA_Num -= 7;
//              }
//            }
//            break;
//            //      case Alphanumeric:
//            //        if (verNum == 1 || verNum == 2 || verNum == 3 || verNum == 4 || verNum == 5 ||
//            //            verNum == 6 ) {
//            //          CountIndicator = Decoedinprocesss(9, DataPosition, DataRemainder);
//            //          DATA_Num -= 9;
//            //          int CountRemainder = CountIndicator % 2;
//            //          unsigned char First_code, Seconed_code;

//            //          CountIndicator /= 2;
//            //          while (CountIndicator > 0) {
//            //            EncodingWord = Decoedinprocesss(11, DataPosition, DataRemainder);
//            //            First_code = EncodingWord / 45;
//            //            Seconed_code = EncodingWord % 45;
//            //            for (int i = 0; i < 45; i++) {
//            //              if (Alphanumeric_Table[i] == First_code) {
//            //                decodedText[EncodeNum] = Alphanumeric_Table[i];
//            //                break;
//            //              }
//            //            }

//            //            EncodeNum++;
//            //            for (int i = 0; i < 45; i++) {
//            //              if (Alphanumeric_Table[i] == Seconed_code) {
//            //                decodedText[EncodeNum] = Alphanumeric_Table[i];
//            //                break;
//            //              }
//            //            }
//            //            EncodeNum++;
//            //            DATA_Num -= 11;
//            //            CountIndicator -= 1;
//            //          }
//            //          if (CountRemainder == 1) {
//            //            EncodingWord = Decoedinprocesss(6, DataPosition, DataRemainder);
//            //            for (int i = 0; i < 45; i++) {
//            //              if (Alphanumeric_Table[i] == Seconed_code) {
//            //                decodedText[EncodeNum] = Alphanumeric_Table[i];
//            //                break;
//            //              }
//            //            }

//            //            DATA_Num -= 6;
//            //            EncodeNum++;
//            //          }
//            //        }
//            //        break;
//          case bit_8:
//            if (verNum == 1 || verNum == 2 || verNum == 3 || verNum == 4 || verNum == 5 ||
//                verNum == 6) {
//              CountIndicator = Decoedinprocesss(8, DataPosition, DataRemainder);
//              DATA_Num -= 8;
//              while (CountIndicator > 0) {
//                EncodingWord = Decoedinprocesss(8, DataPosition, DataRemainder);
//                decodedText[EncodeNum] = EncodingWord;
//                DATA_Num -= 8;
//                CountIndicator -= 1;
//                EncodeNum++;
//              }
//            }
//            break;
//        }
//      }
//}


