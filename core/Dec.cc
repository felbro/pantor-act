#include "Dec.h"

using namespace ACT;

void
DecoderBase::onUnknownMessage (u32 type)
{
   ++ nMsgsIgnored;
}

void
DecoderBase::onStrayBytes (u32 len)
{
   nStrayBytes += len;
}

void
DecoderBase::dumpStats () const
{
   if (nMsgsIgnored == 0 && nStrayBytes == 0)
      return;
       
   std::cerr << "Decoder: " << nMsgsIgnored << " msg(s) ignored, "
             << nStrayBytes << " bytes ignored" << std::endl;
}


// //////////////////////////////////////////////////////////////
// Binary
u16
BinDecoder::decodeU16 (const u8* data, u32 off) const
{
   u16 v = * (reinterpret_cast<const u16*> (data + off));
   return v;
}

u32
BinDecoder::decodeU32 (const u8* data, u32 off) const
{
   u32 v = * (reinterpret_cast<const u32*> (data + off));
   return v;
}

u64
BinDecoder::decodeU64 (const u8* data, u32 off) const
{
   u64 v = * (reinterpret_cast<const u64*> (data + off));
   return v;
}

i64
BinDecoder::decodeI64 (const u8* data, u32 off) const
{
   i64 v = * (reinterpret_cast<const i64*> (data + off));
   return v;
}

std::string
BinDecoder::decodeString (const u8* data, u32 off, u32 maxLen) const
{
   u32 start = off;
   u32 end = off;

   while (maxLen > 0)
   {
      if (data [end] == 0)
         break;
      -- maxLen;
      ++ end;
   }

   return std::string ((const char*) (& data [start]), end - start);
}

// //////////////////////////////////////////////////////////////

void
BinDecoder::onMessage (u32 msgLen, const u8* data)
{
   msgBase.msgSize = msgLen; // (already consumed/read by parser)

   msgBase.msgType = decodeU16 (data, 0);

   // 
   switch (msgBase.msgType)
   {
    case 21: // InsertOrder
    case 22: // DeleteOrder
    case 23: // ReduceOrder
    case 24: // ReplaceOrder
      // not part of msg
      msgBase.msgSeqNo = 0;
      msgBase.msgTsp = 0;
      break;
    default:
      msgBase.msgSeqNo = decodeU32 (data, 2);
      msgBase.msgTsp = decodeU64 (data, 6);
      break;
   }

   u32 b = 0;
   switch (msgBase.msgType)
   {
    case 1: b = decodeSystemStatus (data); break;
    case 2: b = decodeInstrumentInfo (data); break;
    case 3: b = decodeInstrumentStatus (data); break;
    case 4: b = decodeIndexInfo (data); break;
    case 5: b = decodeIndexMemberInfo (data); break;
    case 6: b = decodeIndexUpdate (data); break;
    case 7: b = decodeOrderInserted (data); break;
    case 8: b = decodeOrderDeleted (data); break;
    case 9: b = decodeOrderReduced (data); break;
    case 10: b = decodeOrderReplaced (data); break;
    case 11: b = decodeOrderExecuted (data); break;
    case 12: b = decodeTopOfBook (data); break;
    case 13: b = decodeTradeStats (data); break;
    case 21: b = decodeInsertOrder (data); break;
    case 22: b = decodeDeleteOrder (data); break;
    case 23: b = decodeReduceOrder (data); break;
    case 24: b = decodeReplaceOrder (data); break;
    case 25: b = decodeInsertOrderReply (data); break;
    case 26: b = decodeDeleteOrderReply (data); break;
    case 27: b = decodeReduceOrderReply (data); break;
    case 28: b = decodeReplaceOrderReply (data); break;
    case 29: b = decodeRejectOrderReply (data); break;
    default:
      onUnknownMessage (msgBase.msgType);
      onStrayBytes (msgLen);
      return;
   }

   if (b < msgLen)
      onStrayBytes (msgLen - b);
}

void
BinDecoder::assertMsgSize (const MsgBase & m, u32 sizeRequired) const
{
   if (m.msgSize < sizeRequired)
   {
      std::cerr << "Msg too short: " << m.msgSize << " < " << sizeRequired << std::endl;
      abort ();
   }
}

void
BinDecoder::run ()
{
   size_t n;
   u8 buf [4096];

   while ((n = ::read (0, buf, sizeof (buf))) > 0)
      parser->parse (buf, n);

   dumpStats ();
}

u32
BinDecoder::decodeSystemStatus (const u8* data)
{
   Public::SystemStatus m;
   assertMsgSize (msgBase, m.size ());

   copyHeader (msgBase, m);

   m.open = static_cast<bool> (decodeU32 (data, 14));

   observer.onSystemStatus (m);
   return m.size ();
}

u32
BinDecoder::decodeInstrumentInfo (const u8* data)
{
   Public::InstrumentInfo m;
   assertMsgSize (msgBase, m.size ());

   copyHeader (msgBase, m);

   m.instrumentId = decodeU32 (data, 14);
   m.name = decodeString (data, 18, 16);

   observer.onInstrumentInfo (m);
   return m.size ();
}

u32
BinDecoder::decodeInstrumentStatus (const u8* data)
{
   Public::InstrumentStatus m;
   assertMsgSize (msgBase, m.size ());

   copyHeader (msgBase, m);

   m.instrumentId = decodeU32 (data, 14);
   m.active = static_cast<bool> (decodeU32 (data, 18));

   observer.onInstrumentStatus (m);
   return m.size ();
}

u32
BinDecoder::decodeIndexInfo (const u8* data)
{
   Public::IndexInfo m;
   assertMsgSize (msgBase, m.size ());

   copyHeader (msgBase, m);

   m.indexId = decodeU32 (data, 14);
   m.value = decodeU64 (data, 18);

   u32 v = decodeU32 (data, 26);
   switch (v)
   {
    case 1: m.priceType = PriceType::TradedPrice; break;
    case 2: m.priceType = PriceType::VWAP; break;
    default:
      std::cerr << "IndexInfo: Invalid price type: " << v << " ("
                << msgBase.toString () << ")" << std::endl;
      abort ();
   }
   m.active = static_cast<bool> (decodeU32 (data, 30));
   m.name = decodeString (data, 34, 16);

   observer.onIndexInfo (m);
   return m.size ();
}

u32
BinDecoder::decodeIndexMemberInfo (const u8* data)
{
   Public::IndexMemberInfo m;
   assertMsgSize (msgBase, m.size ());

   copyHeader (msgBase, m);

   m.indexId = decodeU32 (data, 14);
   m.instrumentId = decodeU32 (data, 18);
   m.prevShares = decodeU64 (data, 22);
   m.currShares = decodeU64 (data, 30);
   m.prevPrice = decodeU64 (data, 38);
   m.issueAmount = decodeU64 (data, 46);
   m.dividend = decodeU64 (data, 54);
   m.included = static_cast<bool> (decodeU32 (data, 62));

   observer.onIndexMemberInfo (m);
   return m.size ();
}

u32
BinDecoder::decodeIndexUpdate (const u8* data)
{
   Public::IndexUpdate m;
   assertMsgSize (msgBase, m.size ());

   copyHeader (msgBase, m);

   m.indexId = decodeU32 (data, 14);
   m.value = decodeU64 (data, 18);
      
   observer.onIndexUpdate (m);
   return m.size ();
}

u32
BinDecoder::decodeOrderInserted (const u8* data)
{
   Public::OrderInserted m;
   assertMsgSize (msgBase, m.size ());

   copyHeader (msgBase, m);

   m.instrumentId = decodeU32 (data, 14);
   m.serverOrderId = decodeU64 (data, 18);
   m.price = decodeU64 (data, 26);
   m.quantity = decodeU64 (data, 34);

   u32 v = decodeU32 (data, 42);
   switch (v)
   {
    case 0: m.side = Side::Buy; break;
    case 1: m.side = Side::Sell; break;
    default:
      std::cerr << "Invalid side: " << v << " (" << msgBase.toString () << ")"
                << std::endl;
      abort ();
   }
      
   observer.onOrderInserted (m);
   return m.size ();
}

u32
BinDecoder::decodeOrderDeleted (const u8* data)
{
   Public::OrderDeleted m;
   assertMsgSize (msgBase, m.size ());

   copyHeader (msgBase, m);

   m.instrumentId = decodeU32 (data, 14);
   m.serverOrderId = decodeU64 (data, 18);

   observer.onOrderDeleted (m);
   return m.size ();
}

u32
BinDecoder::decodeOrderReduced (const u8* data)
{
   Public::OrderReduced m;
   assertMsgSize (msgBase, m.size ());

   copyHeader (msgBase, m);

   m.instrumentId = decodeU32 (data, 14);
   m.serverOrderId = decodeU64 (data, 18);
   m.quantity = decodeU64 (data, 26);

   observer.onOrderReduced (m);
   return m.size ();
}

u32
BinDecoder::decodeOrderReplaced (const u8* data)
{
   Public::OrderReplaced m;
   assertMsgSize (msgBase, m.size ());

   copyHeader (msgBase, m);

   m.instrumentId = decodeU32 (data, 14);
   m.serverOrderId = decodeU64 (data, 18);
   m.newServerOrderId = decodeU64 (data, 26);
   m.price = decodeU64 (data, 34);
   m.quantity = decodeU64 (data, 42);
      
   observer.onOrderReplaced (m);
   return m.size ();
}

u32
BinDecoder::decodeOrderExecuted (const u8* data)
{
   Public::OrderExecuted m;
   assertMsgSize (msgBase, m.size ());

   copyHeader (msgBase, m);

   m.instrumentId = decodeU32 (data, 14);
   m.serverOrderId = decodeU64 (data, 18);
   m.price = decodeU64 (data, 26);
   m.quantity = decodeU64 (data, 34);
      
   observer.onOrderExecuted (m);
   return m.size ();
}

u32
BinDecoder::decodeTopOfBook (const u8* data)
{
   Public::TopOfBook m;
   assertMsgSize (msgBase, m.size ());

   copyHeader (msgBase, m);

   m.instrumentId = decodeU32 (data, 14);
   m.bidPrice = decodeU64 (data, 18);
   m.bidQuantity = decodeU64 (data, 26);
   m.askPrice = decodeU64 (data, 34);
   m.askQuantity = decodeU64 (data, 42);

   observer.onTopOfBook (m);
   return m.size ();
}

u32
BinDecoder::decodeTradeStats (const u8* data)
{
   Public::TradeStats m;
   assertMsgSize (msgBase, m.size ());

   copyHeader (msgBase, m);

   m.instrumentId = decodeU32 (data, 14);
   m.lastPrice = decodeU64 (data, 18);
   m.lastQuantity = decodeU64 (data, 26);
   m.vwapPrice = decodeU64 (data, 34);

   observer.onTradeStats (m);
   return m.size ();
}

u32
BinDecoder::decodeInsertOrderReply (const u8* data)
{
   Private::InsertOrderReply m;
   assertMsgSize (msgBase, m.size ());

   copyHeader (msgBase, m);
   u32 v = 0;

   m.instrumentId = decodeU32 (data, 14);
   m.clientId = decodeU32 (data, 18);
   m.clientOrderId = decodeU64 (data, 22);
   m.serverOrderId = decodeU64 (data, 30);
   m.price = decodeU64 (data, 38);
   m.quantity = decodeU64 (data, 46);

   switch (v = decodeU32 (data, 54))
   {
    case 0: m.side = Side::Buy; break;
    case 1: m.side = Side::Sell; break;
    default:
      std::cerr << "Invalid Side:" << v << " ("
                << msgBase.toString () << ")" << std::endl;
      abort ();
   }

   switch (v = decodeU32 (data, 58))
   {
    case 1: m.tif = TimeInForce::IOC; break;
    case 2: m.tif = TimeInForce::GTC; break;
    default:
      std::cerr << "Invalid TIF:" << v << " ("
                << msgBase.toString () << ")" << std::endl;
      abort ();
   }
         
   observer.onInsertOrderReply (m);
   return m.size ();
}

u32
BinDecoder::decodeDeleteOrderReply (const u8* data)
{
   Private::DeleteOrderReply m;
   assertMsgSize (msgBase, m.size ());

   copyHeader (msgBase, m);

   m.instrumentId = decodeU32 (data, 14);
   m.clientId = decodeU32 (data, 18);
   m.clientOrderId = decodeU64 (data, 22);

   observer.onDeleteOrderReply (m);
   return m.size ();
}

u32
BinDecoder::decodeReduceOrderReply (const u8* data)
{
   Private::ReduceOrderReply m;
   assertMsgSize (msgBase, m.size ());

   copyHeader (msgBase, m);

   m.instrumentId = decodeU32 (data, 14);
   m.clientId = decodeU32 (data, 18);
   m.clientOrderId = decodeU64 (data, 22);
   m.quantity = decodeU64 (data, 30);
      
   observer.onReduceOrderReply (m);
   return m.size ();
}

u32
BinDecoder::decodeReplaceOrderReply (const u8* data)
{
   Private::ReplaceOrderReply m;
   assertMsgSize (msgBase, m.size ());

   copyHeader (msgBase, m);

   m.instrumentId = decodeU32 (data, 14);
   m.clientId = decodeU64 (data, 18);
   m.oldClientOrderId = decodeU64 (data, 22);
   m.newClientOrderId = decodeU64 (data, 30);
   m.serverOrderId = decodeU64 (data, 38);
   m.price = decodeU64 (data, 46);
   m.quantity = decodeU64 (data, 54);

   observer.onReplaceOrderReply (m);
   return m.size ();
}

u32
BinDecoder::decodeRejectOrderReply (const u8* data)
{
   Private::RejectOrderReply m;
   assertMsgSize (msgBase, m.size ());

   copyHeader (msgBase, m);

   m.instrumentId = decodeU32 (data, 14);
   m.clientId = decodeU32 (data, 18);
   m.clientOrderId = decodeU64 (data, 22);

   u32 v = decodeU32 (data, 30);
   switch (v)
   {
    case 1: m.reason = RejectReason::InvalidInstrument; break;
    case 2: m.reason = RejectReason::InvalidPrice; break;
    case 3: m.reason = RejectReason::InvalidQuantity; break;
    case 4: m.reason = RejectReason::InvalidOrderId; break;
	case 5: m.reason = RejectReason::InvalidNotionalValue; break;
	case 6: m.reason = RejectReason::NotionalLimitExceeded; break;
    default:
      std::cerr << "Invalid reject reason: " << v << " ("
                << msgBase.toString () << ")" << std::endl;
      abort ();
   }
      
   observer.onRejectOrderReply (m);
   return m.size ();
}

u32
BinDecoder::decodeInsertOrder (const u8* data)
{
   Private::InsertOrder m;
   assertMsgSize (msgBase, m.size ());

   copyHeader (msgBase, m);
   
   m.instrumentId = decodeU32 (data, 2);
   m.clientId = decodeU32 (data, 6);
   m.clientOrderId = decodeU64 (data, 10);
   m.price = decodeU64 (data, 18);
   m.quantity = decodeU64 (data, 26);

   u32 v = decodeU32 (data, 34);
   switch (v)
   {
    case 0: m.side = Side::Buy; break;
    case 1: m.side = Side::Sell; break;
    default:
      std::cerr << "Invalid side: " << v << " ("
                << msgBase.toString () << ")" << std::endl;
      abort ();
   }
      
   v = decodeU32 (data, 38);
   switch (v)
   {
    case 1: m.tif = TimeInForce::IOC; break;
    case 2: m.tif = TimeInForce::GTC; break;
    default:
      std::cerr << "Invalid TIF: " << v << " ("
                << msgBase.toString () << ")" << std::endl;
      abort ();
   }
      
   observer.onInsertOrder (m);
   return m.size ();
}

u32
BinDecoder::decodeDeleteOrder (const u8* data)
{
   Private::DeleteOrder m;
   assertMsgSize (msgBase, m.size ());

   copyHeader (msgBase, m);

   m.instrumentId = decodeU32 (data, 2);
   m.clientId = decodeU32 (data, 6);
   m.clientOrderId = decodeU64 (data, 10);

   observer.onDeleteOrder (m);
   return m.size ();
}

u32
BinDecoder::decodeReduceOrder (const u8* data)
{
   Private::ReduceOrder m;
   assertMsgSize (msgBase, m.size ());

   copyHeader (msgBase, m);

   m.instrumentId = decodeU32 (data, 2);
   m.clientId = decodeU32 (data, 6);
   m.clientOrderId = decodeU64 (data, 10);
   m.quantity = decodeU64 (data, 18);

   observer.onReduceOrder (m);
   return m.size ();
}

u32
BinDecoder::decodeReplaceOrder (const u8* data)
{
   Private::ReplaceOrder m;
   assertMsgSize (msgBase, m.size ());

   copyHeader (msgBase, m);

   m.instrumentId = decodeU32 (data, 2);
   m.clientId = decodeU32 (data, 6);
   m.clientOrderId = decodeU64 (data, 10);
   m.newClientOrderId = decodeU64 (data, 18);
   m.price = decodeU64 (data, 26);
   m.quantity = decodeU64 (data, 34);

   observer.onReplaceOrder (m);
   return m.size ();
}

// //////////////////////////////////////////////////////////////
// Text/Ascii

void
TextDecoder::skipSpaces ()
{
   // skip initial spaces:
   while (msgOff < msgLen)
   {
      if (msgBuf [msgOff] != ' ')
         break;

      ++ msgOff;
   }
}

// (a string may contain spaces)
static inline bool isStringDelimiter (u8 c)
{
   return ((c == '|' || c == '\t' ||
            c == '\n' || c == '#'));
}

// (same as 'string' plus 'space')
static inline bool isDelimiter (u8 c)
{
   return (c == ' ' || isStringDelimiter (c));
}

// unsigned int's:
template<typename T> void
TextDecoder::decode (T & v)
{
   v = 0;

   skipSpaces ();

   while (msgOff < msgLen)
   {
      u8 c = msgBuf [msgOff++];
      if (isDelimiter (c))
         break;

      if (c < '0' || c > '9')
      {
         std::cerr << "decode int: invalid character: '" << (char) c
                   << "'" << std::endl;
         abort ();
      }

      v = v * 10 + (c - '0');
   }
}

// signed int's:
template<typename T> void
TextDecoder::decodeSigned (T & v)
{
   v = 0;

   skipSpaces ();

   u32 pos = 0;
   bool negative = false;

   while (msgOff < msgLen)
   {
      u8 c = msgBuf [msgOff++];
      if (isDelimiter (c))
         break;

      if (pos++ == 0)
      {
         if (c == '-')
         {
            negative = true;
            continue;
         }
      }

      if (c < '0' || c > '9')
      {
         std::cerr << "decode int: invalid character: " << (char) c
                   << "'" << std::endl;
         abort ();
      }

      v = v * 10 + (c - '0');
   }

   if (negative)
      v = -v;
}

// price fields etc
void
TextDecoder::decodeFloat (u64 & v)
{
   v = 0;
   bool seenDecimalPoint = false;
   u64 factor = 10000;
   u32 numDec = 0;

   skipSpaces ();

   while (msgOff < msgLen)
   {
      u8 c = msgBuf [msgOff++];

      if (isDelimiter (c))
         break;

      if (c == '.')
      {
         if (! seenDecimalPoint)
         {
            seenDecimalPoint = true;
            continue;
         }
      }

      if (c < '0' || c > '9')
      {
         std::cerr << "decode int: invalid character: '" << (char) c
                   << "'" << std::endl;
         abort ();
      }

      v = v * 10 + (c - '0');

      if (seenDecimalPoint)
      {
         factor /= 10;
         ++ numDec;
         if (numDec > 4)
         {
            std::cerr << "decode price: too many decimals" << std::endl;
         }
      }
   }

   v *= factor;
}

// strings
void
TextDecoder::decodeStr (std::string & v)
{
   const u8* start = & msgBuf [msgOff];
   u32 n = 0;

   while (msgOff < msgLen)
   {
      char c = msgBuf [msgOff++];
      if (isStringDelimiter (c))
         break;

      ++ n;
   }

   v = std::string ((const char*) start, n);
}

// //////////////////////////////////////////////////////////////

void
TextDecoder::onMessage (const u8* data, u32 len)
{
   // comment line?
   if (*data == '#')
      return;

   // empty line?
   if (len == 0)
      return;

   msgBuf = data;
   msgOff = 0;
   msgLen = len;
   
   msgBase.msgSize = 0; // not used in text mode

   // get the type:
   decode (msgBase.msgType);

   // 
   switch (msgBase.msgType)
   {
    case 21: // InsertOrder
    case 22: // DeleteOrder
    case 23: // ReduceOrder
    case 24: // ReplaceOrder
      // not part of msg
      msgBase.msgSeqNo = 0;
      msgBase.msgTsp = 0;
      break;
    default:
      decode (msgBase.msgSeqNo);
      decode (msgBase.msgTsp);
      break;
   }

   switch (msgBase.msgType)
   {
    case 1: decodeSystemStatus (); break;
    case 2: decodeInstrumentInfo (); break;
    case 3: decodeInstrumentStatus (); break;
    case 4: decodeIndexInfo (); break;
    case 5: decodeIndexMemberInfo (); break;
    case 6: decodeIndexUpdate (); break;
    case 7: decodeOrderInserted (); break;
    case 8: decodeOrderDeleted (); break;
    case 9: decodeOrderReduced (); break;
    case 10: decodeOrderReplaced (); break;
    case 11: decodeOrderExecuted (); break;
    case 12: decodeTopOfBook (); break;
    case 13: decodeTradeStats (); break;
    case 21: decodeInsertOrder (); break;
    case 22: decodeDeleteOrder (); break;
    case 23: decodeReduceOrder (); break;
    case 24: decodeReplaceOrder (); break;
    case 25: decodeInsertOrderReply (); break;
    case 26: decodeDeleteOrderReply (); break;
    case 27: decodeReduceOrderReply (); break;
    case 28: decodeReplaceOrderReply (); break;
    case 29: decodeRejectOrderReply (); break;
    default:
      onUnknownMessage (msgBase.msgType);
      onStrayBytes (len);
      return;
   }

   onStrayBytes (len - msgOff);
}

void
TextDecoder::run ()
{
   size_t n;
   u8 buf [4096];

   while ((n = ::read (0, buf, sizeof (buf))) > 0)
      parser->parse (buf, n);

   dumpStats ();
}

void
TextDecoder::decodeSystemStatus ()
{
   Public::SystemStatus m;
   copyHeader (msgBase, m);

   u32 v = 0;
   decode (v);

   m.open = static_cast<bool> (v);

   observer.onSystemStatus (m);
}

void
TextDecoder::decodeInstrumentInfo ()
{
   Public::InstrumentInfo m;
   copyHeader (msgBase, m);

   decode (m.instrumentId);
   decodeStr (m.name);

   observer.onInstrumentInfo (m);
}

void
TextDecoder::decodeInstrumentStatus ()
{
   Public::InstrumentStatus m;
   copyHeader (msgBase, m);

   decode (m.instrumentId);
   u32 v = 0;
   decode (v);
   m.active = static_cast<bool> (v);

   observer.onInstrumentStatus (m);
}

void
TextDecoder::decodeIndexInfo ()
{
   Public::IndexInfo m;
   copyHeader (msgBase, m);

   decode (m.indexId);
   decodeFloat (m.value);

   u32 v = 0;
   decode (v);
   switch (v)
   {
    case 1: m.priceType = PriceType::TradedPrice; break;
    case 2: m.priceType = PriceType::VWAP; break;
    default:
      std::cerr << "Invalid price type: " << v << " ("
                << msgBase.toString () << ")" << std::endl;
      abort ();
   }
   decode (v);
   m.active = static_cast<bool> (v);
   decodeStr (m.name);

   observer.onIndexInfo (m);
}

void
TextDecoder::decodeIndexMemberInfo ()
{
   Public::IndexMemberInfo m;
   copyHeader (msgBase, m);

   decode (m.indexId);
   decode (m.instrumentId);
   decode (m.prevShares);
   decode (m.currShares);
   decodeFloat (m.prevPrice);
   decodeFloat (m.issueAmount);
   decodeFloat (m.dividend);
   u32 v = 0;
   decode (v);
   m.included = static_cast<bool> (v);

   observer.onIndexMemberInfo (m);
}

void
TextDecoder::decodeIndexUpdate ()
{
   Public::IndexUpdate m;
   copyHeader (msgBase, m);

   decode (m.indexId);
   decodeFloat (m.value);
      
   observer.onIndexUpdate (m);
}

void
TextDecoder::decodeOrderInserted ()
{
   Public::OrderInserted m;
   copyHeader (msgBase, m);

   decode (m.instrumentId);
   decode (m.serverOrderId);
   decodeFloat (m.price);
   decode (m.quantity);

   u32 v = 0;
   decode (v);
   switch (v)
   {
    case 0: m.side = Side::Buy; break;
    case 1: m.side = Side::Sell; break;
    default:
      std::cerr << "Invalid side: " << v << " (" << msgBase.toString ()
                << ")" << std::endl;
      abort ();
   }
      
   observer.onOrderInserted (m);
}

void
TextDecoder::decodeOrderDeleted ()
{
   Public::OrderDeleted m;
   copyHeader (msgBase, m);

   decode (m.instrumentId);
   decode (m.serverOrderId);

   observer.onOrderDeleted (m);
}

void
TextDecoder::decodeOrderReduced ()
{
   Public::OrderReduced m;
   copyHeader (msgBase, m);

   decode (m.instrumentId);
   decode (m.serverOrderId);
   decode (m.quantity);

   observer.onOrderReduced (m);
}

void
TextDecoder::decodeOrderReplaced ()
{
   Public::OrderReplaced m;
   copyHeader (msgBase, m);

   decode (m.instrumentId);
   decode (m.serverOrderId);
   decode (m.newServerOrderId);
   decodeFloat (m.price);
   decode (m.quantity);
      
   observer.onOrderReplaced (m);
}

void
TextDecoder::decodeOrderExecuted ()
{
   Public::OrderExecuted m;
   copyHeader (msgBase, m);

   decode (m.instrumentId);
   decode (m.serverOrderId);
   decodeFloat (m.price);
   decode (m.quantity);
      
   observer.onOrderExecuted (m);
}

void
TextDecoder::decodeTopOfBook ()
{
   Public::TopOfBook m;
   copyHeader (msgBase, m);

   decode (m.instrumentId);
   decodeFloat (m.bidPrice);
   decode (m.bidQuantity);
   decodeFloat (m.askPrice);
   decode (m.askQuantity);

   observer.onTopOfBook (m);
}

void
TextDecoder::decodeTradeStats ()
{
   Public::TradeStats m;
   copyHeader (msgBase, m);

   decode (m.instrumentId);
   decodeFloat (m.lastPrice);
   decode (m.lastQuantity);
   decodeFloat (m.vwapPrice);

   observer.onTradeStats (m);
}

void
TextDecoder::decodeInsertOrderReply ()
{
   Private::InsertOrderReply m;
   copyHeader (msgBase, m);
   u32 v = 0;
      
   decode (m.instrumentId);
   decode (m.clientId);
   decode (m.clientOrderId);
   decode (m.serverOrderId);
   decodeFloat (m.price);
   decode (m.quantity);
   decode (v);
   switch (v)
   {
    case 0: m.side = Side::Buy; break;
    case 1: m.side = Side::Sell; break;
    default:
      std::cerr << "Invalid Side:" << v << " ("
                << msgBase.toString () << ")" << std::endl;
      abort ();
   }

   decode (v);
   switch (v)
   {
    case 1: m.tif = TimeInForce::IOC; break;
    case 2: m.tif = TimeInForce::GTC; break;
    default:
      std::cerr << "Invalid TIF:" << v << " ("
                << msgBase.toString () << ")" << std::endl;
      abort ();
   }
         
   observer.onInsertOrderReply (m);
}

void
TextDecoder::decodeDeleteOrderReply ()
{
   Private::DeleteOrderReply m;
   copyHeader (msgBase, m);

   decode (m.instrumentId);
   decode (m.clientId);
   decode (m.clientOrderId);

   observer.onDeleteOrderReply (m);
}

void
TextDecoder::decodeReduceOrderReply ()
{
   Private::ReduceOrderReply m;
   copyHeader (msgBase, m);

   decode (m.instrumentId);
   decode (m.clientId);
   decode (m.clientOrderId);
   decode (m.quantity);
      
   observer.onReduceOrderReply (m);
}

void
TextDecoder::decodeReplaceOrderReply ()
{
   Private::ReplaceOrderReply m;
   copyHeader (msgBase, m);

   decode (m.instrumentId);
   decode (m.clientId);
   decode (m.oldClientOrderId);
   decode (m.newClientOrderId);
   decode (m.serverOrderId);
   decodeFloat (m.price);
   decode (m.quantity);

   observer.onReplaceOrderReply (m);
}

void
TextDecoder::decodeRejectOrderReply ()
{
   Private::RejectOrderReply m;
   copyHeader (msgBase, m);

   decode (m.instrumentId);
   decode (m.clientId);
   decode (m.clientOrderId);
   u32 v;
   decode (v);
   switch (v)
   {
    case 1: m.reason = RejectReason::InvalidInstrument; break;
    case 2: m.reason = RejectReason::InvalidPrice; break;
    case 3: m.reason = RejectReason::InvalidQuantity; break;
    case 4: m.reason = RejectReason::InvalidOrderId; break;
	case 5: m.reason = RejectReason::InvalidNotionalValue; break;
	case 6: m.reason = RejectReason::NotionalLimitExceeded; break;
    default:
      std::cerr << "Invalid reject reason: " << v << " ("
                << msgBase.toString () << ")" << std::endl;
      abort ();
   }

   observer.onRejectOrderReply (m);
}

void
TextDecoder::decodeInsertOrder ()
{
   Private::InsertOrder m;
   copyHeader (msgBase, m);

   decode (m.instrumentId);
   decode (m.clientId);
   decode (m.clientOrderId);
   decodeFloat (m.price);
   decode (m.quantity);

   u32 v = 0;
   decode (v);
   switch (v)
   {
    case 0: m.side = Side::Buy; break;
    case 1: m.side = Side::Sell; break;
    default:
      std::cerr << "Invalid side: " << v << " ("
                << msgBase.toString () << ")" << std::endl;
      abort ();
   }
      
   decode (v);
   switch (v)
   {
    case 1: m.tif = TimeInForce::IOC; break;
    case 2: m.tif = TimeInForce::GTC; break;
    default:
      std::cerr << "Invalid TIF: " << v << " ("
                << msgBase.toString () << ")" << std::endl;
      abort ();
   }
      
   observer.onInsertOrder (m);
}

void
TextDecoder::decodeDeleteOrder ()
{
   Private::DeleteOrder m;
   copyHeader (msgBase, m);

   decode (m.instrumentId);
   decode (m.clientId);
   decode (m.clientOrderId);

   observer.onDeleteOrder (m);
}

void
TextDecoder::decodeReduceOrder ()
{
   Private::ReduceOrder m;
   copyHeader (msgBase, m);

   decode (m.instrumentId);
   decode (m.clientId);
   decode (m.clientOrderId);
   decode (m.quantity);
   
   observer.onReduceOrder (m);
}

void
TextDecoder::decodeReplaceOrder ()
{
   Private::ReplaceOrder m;
   copyHeader (msgBase, m);

   decode (m.instrumentId);
   decode (m.clientId);
   decode (m.clientOrderId);
   decode (m.newClientOrderId);
   decodeFloat (m.price);
   decodeSigned (m.quantity);

   observer.onReplaceOrder (m);
}

