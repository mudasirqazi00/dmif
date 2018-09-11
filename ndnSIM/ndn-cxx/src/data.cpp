/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013-2017 Regents of the University of California.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 *
 * ndn-cxx library is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * ndn-cxx library is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 * You should have received copies of the GNU General Public License and GNU Lesser
 * General Public License along with ndn-cxx, e.g., in COPYING.md file.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 */

#include "data.hpp"
#include "encoding/block-helpers.hpp"
#include "util/sha256.hpp"
#include "../src/ndnSIM/LogManager.cpp"

namespace ndn {

BOOST_CONCEPT_ASSERT((boost::EqualityComparable<Data>));
BOOST_CONCEPT_ASSERT((WireEncodable<Data>));
BOOST_CONCEPT_ASSERT((WireEncodableWithEncodingBuffer<Data>));
BOOST_CONCEPT_ASSERT((WireDecodable<Data>));
static_assert(std::is_base_of<tlv::Error, Data::Error>::value,
              "Data::Error must inherit from tlv::Error");

Data::Data(const Name& name)
  : m_name(name)
  , m_content(tlv::Content)
{
}

Data::Data(const Block& wire)
{
  wireDecode(wire);
}

template<::ndn::encoding::Tag TAG>
size_t Data::wireEncodeIdsList(EncodingImpl<TAG>& encoder) const {
//	LogManager::AddLogWithNodeId("data.cpp->wireEncodeIdsList.start");
	size_t totalLength = 0;
	for (unsigned int i = 0; i < (unsigned int) m_idsList.size(); i++) {
		uint32_t val = m_idsList[i];
		totalLength += encoder.prependByteArray(reinterpret_cast<uint8_t*>(&val), sizeof(val));
		totalLength += encoder.prependVarNumber(sizeof(val));
	}

	totalLength += encoder.prependVarNumber(totalLength);
	totalLength += encoder.prependVarNumber(::ndn::tlv::IdsList);
//	LogManager::AddLogWithNodeId("data.cpp->wireEncodeIdsList.completed");
	return totalLength;
}

template<encoding::Tag TAG>
size_t Data::wireEncode(EncodingImpl<TAG>& encoder, bool wantUnsignedPortionOnly) const {
	// Data ::= DATA-TLV TLV-LENGTH
	//            Name
	//            MetaInfo
	//            Content
	//            SignatureInfo
	//            SignatureValue

//	LogManager::AddLogWithNodeId("data.cpp->wireEncode.start");

	size_t totalLength = 0;

	// SignatureValue
	if (!wantUnsignedPortionOnly) {
		if (!m_signature) {
			BOOST_THROW_EXCEPTION(
					Error(
							"Requested wire format, but Data has not been signed"));
		}
		totalLength += encoder.prependBlock(m_signature.getValue());
	}

	// SignatureInfo
	totalLength += encoder.prependBlock(m_signature.getInfo());

	// Content
	totalLength += encoder.prependBlock(getContent());

	// MetaInfo
	totalLength += getMetaInfo().wireEncode(encoder);

	// Name
	totalLength += getName().wireEncode(encoder);

	// Residual Energy
	uint32_t residualEnergy = this->getResidualEnergy();
	totalLength += encoder.prependByteArray(
			reinterpret_cast<uint8_t*>(&residualEnergy),
			sizeof(residualEnergy));
	totalLength += encoder.prependVarNumber(sizeof(residualEnergy));
	totalLength += encoder.prependVarNumber(tlv::ResidualEnergy);

// Initial Hop
	uint32_t initialHop = this->getInitialHop();
	totalLength += encoder.prependByteArray(
			reinterpret_cast<uint8_t*>(&initialHop), sizeof(initialHop));
	totalLength += encoder.prependVarNumber(sizeof(initialHop));
	totalLength += encoder.prependVarNumber(tlv::InitialHop);

// Ids List
	totalLength += wireEncodeIdsList(encoder);

	if (!wantUnsignedPortionOnly) {
		totalLength += encoder.prependVarNumber(totalLength);
		totalLength += encoder.prependVarNumber(tlv::Data);
	}

//	LogManager::AddLogWithNodeId("data.cpp->wireEncode.completed");
	return totalLength;
}

template size_t
Data::wireEncode<encoding::EncoderTag>(EncodingBuffer&, bool) const;

template size_t
Data::wireEncode<encoding::EstimatorTag>(EncodingEstimator&, bool) const;

const Block&
Data::wireEncode(EncodingBuffer& encoder, const Block& signatureValue) const
{
  size_t totalLength = encoder.size();
  totalLength += encoder.appendBlock(signatureValue);

  encoder.prependVarNumber(totalLength);
  encoder.prependVarNumber(tlv::Data);

  const_cast<Data*>(this)->wireDecode(encoder.block());
  return m_wire;
}

const Block&
Data::wireEncode() const
{
	/* Commented because we want to be wireEncoded eveytime
  if (m_wire.hasWire())
    return m_wire;
	 */
  EncodingEstimator estimator;
  size_t estimatedSize = wireEncode(estimator);

  EncodingBuffer buffer(estimatedSize, 0);
  wireEncode(buffer);

  const_cast<Data*>(this)->wireDecode(buffer.block());
  return m_wire;
}

std::vector<int> Data::wireDecodeIdsList(const Block& wire) {
//	LogManager::AddLogWithNodeId("data.cpp->wireDecodeIdsList.start");
	std::vector<int> v;
	const Block& content = wire; //m_wire.find(tlv::IdsList);
	content.parse();
	auto element = content.elements_begin();
	if (content.elements_end() == element) {
		return v;
	}

	for (; element != content.elements_end(); ++element) {
		element->parse();
		uint32_t temp = 0;
		std::memcpy(&temp, element->value(), sizeof(temp));
		v.push_back(temp);
	}

//	LogManager::AddLogWithNodeId("data.cpp->wireDecodeIdsList.completed");
	return v;
}

void
Data::wireDecode(const Block& wire) {
//	LogManager::AddLogWithNodeId("data.cpp->wireDecode.start");
	m_fullName.clear();
	m_wire = wire;
	m_wire.parse();

	// Name
	m_name.wireDecode(m_wire.get(tlv::Name));

	// MetaInfo
	m_metaInfo.wireDecode(m_wire.get(tlv::MetaInfo));

	// Content
	m_content = m_wire.get(tlv::Content);

	// SignatureInfo
	m_signature.setInfo(m_wire.get(tlv::SignatureInfo));

	// SignatureValue
	Block::element_const_iterator val = m_wire.find(tlv::SignatureValue);
	if (val != m_wire.elements_end()) {
		m_signature.setValue(*val);
	}

//	LogManager::AddLogWithNodeId("data.cpp->wireDecode.1");
	// Residual Energy
	try{
		val = m_wire.find(tlv::ResidualEnergy);
		if (val == m_wire.elements_end()) {
			BOOST_THROW_EXCEPTION(Error("ResidualEnergy element is missing"));
		}
		uint32_t residualEnergy = 0;
		if (val->value_size() != sizeof(residualEnergy)) {
			BOOST_THROW_EXCEPTION(Error("ResidualEnergy element is malformed"));
		}
		std::memcpy(&residualEnergy, val->value(), sizeof(residualEnergy));
		m_residualEnergy = residualEnergy;
	}
	catch(std::exception& ex){
//		LogManager::AddLogWithNodeId("data.cpp->ResidualEnergy.exception", ex.what());
	}

//	LogManager::AddLogWithNodeId("data.cpp->wireDecode.2");
	// Initial Hop
	try{
		val = m_wire.find(tlv::InitialHop);
		if (val == m_wire.elements_end()) {
			BOOST_THROW_EXCEPTION(Error("ResidualEnergy element is missing"));
		}
		uint32_t initialHop = 0;
		if (val->value_size() != sizeof(initialHop)) {
			BOOST_THROW_EXCEPTION(Error("InitialHop element is malformed"));
		}
		std::memcpy(&initialHop, val->value(), sizeof(initialHop));
		m_initialHop = initialHop;
	}
	catch(std::exception& ex){
//		LogManager::AddLogWithNodeId("data.cpp->InitialHop.exception", ex.what());
	}

//	LogManager::AddLogWithNodeId("data.cpp->wireDecode.3");
	// Ids List
	try{
		val = m_wire.find(tlv::IdsList);
		if (val == m_wire.elements_end()) {
			BOOST_THROW_EXCEPTION(Error("Ids List element is missing"));
		}
		std::vector<int> v = wireDecodeIdsList(*val);
		m_idsList = v;
	}
	catch(std::exception& ex){
//		LogManager::AddLogWithNodeId("data.cpp->IdsList.exception", ex.what());
	}

//	LogManager::AddLogWithNodeId("data.cpp->wireDecode.completed");
}

const Name&
Data::getFullName() const
{
  if (m_fullName.empty()) {
    if (!m_wire.hasWire()) {
      BOOST_THROW_EXCEPTION(Error("Cannot compute full name because Data has no wire encoding (not signed)"));
    }
    m_fullName = m_name;
    m_fullName.appendImplicitSha256Digest(util::Sha256::computeDigest(m_wire.wire(), m_wire.size()));
  }

  return m_fullName;
}

void
Data::resetWire()
{
  m_wire.reset();
  m_fullName.clear();
}

Data&
Data::setName(const Name& name)
{
  resetWire();
  m_name = name;
  return *this;
}

Data&
Data::setMetaInfo(const MetaInfo& metaInfo)
{
  resetWire();
  m_metaInfo = metaInfo;
  return *this;
}

const Block&
Data::getContent() const
{
  if (!m_content.hasWire()) {
    const_cast<Block&>(m_content).encode();
  }
  return m_content;
}

Data&
Data::setContent(const Block& block)
{
  resetWire();

  if (block.type() == tlv::Content) {
    m_content = block;
  }
  else {
    m_content = Block(tlv::Content, block);
  }

  return *this;
}

Data&
Data::setContent(const uint8_t* value, size_t valueSize)
{
  resetWire();
  m_content = makeBinaryBlock(tlv::Content, value, valueSize);
  return *this;
}

Data&
Data::setContent(const ConstBufferPtr& value)
{
  resetWire();
  m_content = Block(tlv::Content, value);
  return *this;
}

Data&
Data::setSignature(const Signature& signature)
{
  resetWire();
  m_signature = signature;
  return *this;
}

Data&
Data::setSignatureValue(const Block& value)
{
  resetWire();
  m_signature.setValue(value);
  return *this;
}

Data&
Data::setContentType(uint32_t type)
{
  resetWire();
  m_metaInfo.setType(type);
  return *this;
}

Data&
Data::setFreshnessPeriod(const time::milliseconds& freshnessPeriod)
{
  resetWire();
  m_metaInfo.setFreshnessPeriod(freshnessPeriod);
  return *this;
}

Data&
Data::setFinalBlockId(const name::Component& finalBlockId)
{
  resetWire();
  m_metaInfo.setFinalBlockId(finalBlockId);
  return *this;
}

/***** DMIF ******/
Data&
Data::setResidualEnergy(const uint32_t val){
	resetWire();
	m_residualEnergy = val;
	return *this;
}

uint32_t
Data::getResidualEnergy() const {
	return m_residualEnergy;
}

Data&
Data::setInitialHop(const uint32_t val){
	resetWire();
	m_initialHop = val;
	return *this;
}

uint32_t
Data::getInitialHop() const {
	return m_initialHop;
}

Data&
Data::setForwarderId(const uint32_t val){
	resetWire();
	m_forwarderId = val;
	return *this;
}

uint32_t
Data::getForwarderId() const {
	return m_forwarderId;
}

Data&
Data::setIdsList(const std::vector<int> v) {
	resetWire();
	m_idsList = v;
	return *this;
}

std::vector<int>
Data::getIdsList() const {
	return m_idsList;
}

Data&
Data::addIdInIdsList(const int id){
	resetWire();
	m_idsList.push_back(id);
	return *this;
}

std::string
Data::getIdsListStr() const {
	std::string temp = "";
	for(unsigned int i = 0; i < (unsigned int)m_idsList.size(); i++){
		temp += m_idsList[i] + ",";
	}

	if(temp.size() > 0)
		temp = temp.substr(0, temp.size()-1);

	return temp;
}
/***** DMIF ******/

bool
operator==(const Data& lhs, const Data& rhs)
{
  return lhs.getName() == rhs.getName() &&
         lhs.getMetaInfo() == rhs.getMetaInfo() &&
         lhs.getContent() == rhs.getContent() &&
         lhs.getSignature() == rhs.getSignature();
}

std::ostream&
operator<<(std::ostream& os, const Data& data)
{
  os << "Name: " << data.getName() << "\n";
  os << "MetaInfo: " << data.getMetaInfo() << "\n";
  os << "Content: (size: " << data.getContent().value_size() << ")\n";
  os << "Signature: (type: " << data.getSignature().getType()
     << ", value_length: "<< data.getSignature().getValue().value_size() << ")";
  os << "ResidualEnergy: " << data.getResidualEnergy() << "\n";
  os << "InitialHop: " << data.getInitialHop() << "\n";
  os << "IdsList: " << data.getIdsListStr() << "\n";
  os << std::endl;

  return os;
}

} // namespace ndn
