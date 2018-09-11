/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2014-2016,  Regents of the University of California,
 *                           Arizona Board of Regents,
 *                           Colorado State University,
 *                           University Pierre & Marie Curie, Sorbonne University,
 *                           Washington University in St. Louis,
 *                           Beijing Institute of Technology,
 *                           The University of Memphis.
 *
 * This file is part of NFD (Named Data Networking Forwarding Daemon).
 * See AUTHORS.md for complete list of NFD authors and contributors.
 *
 * NFD is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * NFD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * NFD, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "fib-entry.hpp"
#include "../src/ndnSIM/LogManager.cpp"
#include <exception>

namespace nfd {
namespace fib {

Entry::Entry(const Name& prefix)
  : m_prefix(prefix)
  , m_nameTreeEntry(nullptr)
{
//	m_forwarderId = -1;
}

NextHopList::iterator
Entry::findNextHop(const Face& face)
{
  return std::find_if(m_nextHops.begin(), m_nextHops.end(),
                      [&face] (const NextHop& nexthop) {
                        return &nexthop.getFace() == &face;
                      });
}

bool
Entry::hasNextHop(const Face& face) const
{
  return const_cast<Entry*>(this)->findNextHop(face) != m_nextHops.end();
}

void
Entry::addNextHop(Face& face, uint64_t cost)
{
  auto it = this->findNextHop(face);
  if (it == m_nextHops.end()) {
    m_nextHops.emplace_back(face);
    it = std::prev(m_nextHops.end());
  }

  it->setCost(cost);
  this->sortNextHops();
}

void
Entry::removeNextHop(const Face& face)
{
  auto it = this->findNextHop(face);
  if (it != m_nextHops.end()) {
    m_nextHops.erase(it);
  }
}

void
Entry::sortNextHops()
{
  std::sort(m_nextHops.begin(), m_nextHops.end(),
            [] (const NextHop& a, const NextHop& b) { return a.getCost() < b.getCost(); });
}

/************ DMIF *************/
void
Entry::setForwarderId(const uint32_t id){
//	m_nameTreeEntry->setForwarderId(id);
}
uint32_t
Entry::getForwarderId() const {
	return 1;
//	LogManager::AddLogWithNodeId("fib-entry.cpp->getForwarderId");
//	try {
//		uint32_t fi = m_nameTreeEntry->getForwarderId();
//		LogManager::AddLogWithNodeId("fib-entry.cpp->getForwarderId.m_forwarderId", fi);
//		return fi;
//	}
//	catch(const std::exception& ex) {
//		LogManager::AddLogWithNodeId("fib-entry.cpp->getForwarderId.exception", ex.what());
//		return -1;
//	}
}
name_tree::Entry*
Entry::getNameTreeEntry() const {
	return m_nameTreeEntry;
}
/************ DMIF *************/

} // namespace fib
} // namespace nfd
