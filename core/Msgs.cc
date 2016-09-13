#include "Msgs.h"

using namespace ACT;

std::string
MsgBase::toString () const
{
   std::ostringstream os;
   os << "MsgSize=" << msgSize << "|"
      << "MsgType=" << msgType << "|"
      << "MsgSeqNo=" << msgSeqNo << "|"
      << "MsgTsp=" << msgTsp;
   return os.str ();
}

std::string 
Public::SystemStatus::toString () const
{
   std::ostringstream os;
   os << "Status=" << (open ? "Open" : "Closed");
   return os.str ();
}

std::string
Public::InstrumentInfo::toString () const
{
   std::ostringstream os;
   os << "InstrumentId=" << instrumentId << "|"
      << "Name=" << name;
   return os.str ();
}

std::string
Public::InstrumentStatus::toString () const
{
   std::ostringstream os;
   os << "InstrumentId=" << instrumentId << "|"
      << "Status=" << (active ? "Active" : "Stopped");
   return os.str ();
}

std::string
Public::IndexInfo::toString () const
{
   std::ostringstream os;
   os << "IndexId=" << indexId << "|"
      << "Value=" << value << "|"
      << "PriceType="
      << (priceType == PriceType::VWAP ? "VWAP" : "Traded" ) << "|"
      << "Status=" << (active ? "Active" : "Stopped") << "|"
      << "Name=" << name;
   return os.str ();
}

std::string
Public::IndexMemberInfo::toString () const
{
   std::ostringstream os;
   os << "IndexId=" << indexId << "|"
      << "InstrumentId=" << instrumentId << "|"
      << "PrevShares=" << prevShares << "|"
      << "CurrShares=" << currShares << "|"
      << "PrevPrice=" << prevPrice << "|"
      << "IssueAmount=" << issueAmount << "|"
      << "Dividend=" << dividend << "|"
      << "Included=" << (included ? "Yes" : "No");
   return os.str ();
}

std::string
Public::IndexUpdate::toString () const
{
   std::ostringstream os;
   os << "IndexId=" << indexId << "|"
      << "Value=" << value;
   return os.str ();
}

std::string
Public::OrderInserted::toString () const
{
   std::ostringstream os;
   os << "InstrumentId=" << instrumentId << "|"
      << "ServerOrderId=" << serverOrderId << "|"
      << "Price=" << price << "|"
      << "Quantity=" << quantity << "|"
      << "Side=" << (side == Side::Buy ? "Buy" : "Sell");
   return os.str ();
}

std::string
Public::OrderDeleted::toString () const
{
   std::ostringstream os;
   os << "InstrumentId=" << instrumentId << "|"
      << "ServerOrderId=" << serverOrderId;
   return os.str ();
}

std::string
Public::OrderReduced::toString () const
{
   std::ostringstream os;
   os << "InstrumentId=" << instrumentId << "|"
      << "ServerOrderId=" << serverOrderId << "|"
      << "Quantity=" << quantity;
   return os.str ();
}

std::string
Public::OrderReplaced::toString () const
{
   std::ostringstream os;
   os << "InstrumentId=" << instrumentId << "|"
      << "ServerOrderId=" << serverOrderId << "|"
      << "NewServerOrderId=" << newServerOrderId << "|"
      << "Price=" << price << "|"
      << "Quantity=" << quantity;
   return os.str ();
}

std::string
Public::OrderExecuted::toString () const
{
   std::ostringstream os;
   os << "InstrumentId=" << instrumentId << "|"
      << "ServerOrderId=" << serverOrderId << "|"
      << "Price=" << price << "|"
      << "Quantity=" << quantity;
   return os.str ();
}

std::string
Public::TopOfBook::toString () const
{
   std::ostringstream os;
   os << "InstrumentId=" << instrumentId << "|"
      << "BidPrice=" << bidPrice << "|"
      << "BidQty=" << bidQuantity << "|"
      << "AskPrice=" << askPrice << "|"
      << "AskQty=" << askQuantity;
   return os.str ();
}

std::string
Public::TradeStats::toString () const
{
   std::ostringstream os;
   os << "InstrumentId=" << instrumentId << "|"
      << "LastPrice=" << lastPrice << "|"
      << "LastQty=" << lastQuantity << "|"
      << "VwapPrice=" << vwapPrice;
   return os.str ();
}

std::string
Private::InsertOrder::toString () const
{
   std::ostringstream os;
   os << "InstrumentId=" << instrumentId << "|"
      << "ClientId=" << clientId << "|"
      << "ClientOrderId=" << clientOrderId << "|"
      << "Price=" << price << "|"
      << "Qty=" << quantity << "|"
      << "Side=" << (side == Side::Buy ? "Buy" : "Sell") << "|"
      << "TIF=" << (tif == TimeInForce::IOC ? "IOC" : "GTC");
   return os.str ();
}

std::string
Private::DeleteOrder::toString () const
{
   std::ostringstream os;
   os << "InstrumentId=" << instrumentId << "|"
      << "ClientId" << clientId << "|"
      << "ClientOrderId=" << clientOrderId;
   return os.str ();
}

std::string
Private::ReduceOrder::toString () const
{
   std::ostringstream os;
   os << "InstrumentId=" << instrumentId << "|"
      << "ClientId=" << clientId << "|"
      << "ClientOrderId=" << clientOrderId << "|"
      << "Quantity=" << quantity;
   return os.str ();
}

std::string
Private::ReplaceOrder::toString () const
{
   std::ostringstream os;
   os << "InstrumentId=" << instrumentId << "|"
      << "ClientId=" << clientId << "|"
      << "ClientOrderId=" << clientOrderId << "|"
      << "NewClientOrderId=" << newClientOrderId << "|"
      << "Price=" << price << "|"
      << "Quantity=" << quantity;
   return os.str ();
}

std::string
Private::InsertOrderReply::toString () const
{
   std::ostringstream os;
   os << "InstrumentId=" << instrumentId << "|"
      << "ClientId=" << clientId << "|"
      << "ClientOrderId=" << clientOrderId << "|"
      << "ServerOrderId=" << serverOrderId << "|"
      << "Price=" << price << "|"
      << "Quantity=" << quantity << "|"
      << "Side=" << (side == Side::Buy ? "Buy" : "Sell") << "|"
      << "TIF=" << (tif == TimeInForce::IOC ? "IOC" : "GTC");
   return os.str ();
}

std::string
Private::DeleteOrderReply::toString () const
{
   std::ostringstream os;
   os << "InstrumentId=" << instrumentId << "|"
      << "ClientId=" << clientId << "|"
      << "ClientOrderId=" << clientOrderId;
   return os.str ();
}

std::string
Private::ReduceOrderReply::toString () const
{
   std::ostringstream os;
   os << "InstrumentId=" << instrumentId << "|"
      << "ClientId=" << clientId << "|"
      << "ClientOrderId=" << clientOrderId << "|"
      << "Quantity=" << quantity;
   return os.str ();
}

std::string
Private::ReplaceOrderReply::toString () const
{
   std::ostringstream os;
   os << "InstrumentId=" << instrumentId << "|"
      << "ClientId=" << clientId << "|"
      << "OldClientOrderId=" << oldClientOrderId << "|"
      << "NewClientOrderId=" << newClientOrderId << "|"
      << "ServerOrderId=" << serverOrderId << "|"
      << "Price=" << price << "|"
      << "Quantity=" << quantity;
   return os.str ();
}

std::string
Private::RejectOrderReply::toString () const
{
   std::ostringstream os;
   os << "InstrumentId=" << instrumentId << "|"
      << "ClientId=" << clientId << "|"
      << "ClientOrderId=" << clientOrderId << "|"
      << "RejectReason=" << (int) reason;
   return os.str ();
}
