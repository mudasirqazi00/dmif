#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"

#include "ns3/ndnSIM-module.h"
#include "ns3/position-allocator.h"

using namespace std;
namespace ns3 {

NS_LOG_COMPONENT_DEFINE("ndn.WifiExample");

int main(int argc, char* argv[]) {
	// disable fragmentation
	Config::SetDefault("ns3::WifiRemoteStationManager::FragmentationThreshold", StringValue("2200"));
	Config::SetDefault("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue("2200"));
	Config::SetDefault("ns3::WifiRemoteStationManager::NonUnicastMode", StringValue("OfdmRate24Mbps"));

	CommandLine cmd;
	cmd.Parse(argc, argv);

	WifiHelper wifi = WifiHelper::Default();
	wifi.SetStandard(WIFI_PHY_STANDARD_80211a);
	wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager", "DataMode", StringValue("OfdmRate24Mbps"));

	YansWifiChannelHelper wifiChannel;
	wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
	wifiChannel.AddPropagationLoss("ns3::ThreeLogDistancePropagationLossModel");
	wifiChannel.AddPropagationLoss("ns3::NakagamiPropagationLossModel");

	YansWifiPhyHelper wifiPhyHelper = YansWifiPhyHelper::Default();
	wifiPhyHelper.SetChannel(wifiChannel.Create());
	wifiPhyHelper.Set("TxPowerStart", DoubleValue(5));
	wifiPhyHelper.Set("TxPowerEnd", DoubleValue(5));

	NqosWifiMacHelper wifiMacHelper = NqosWifiMacHelper::Default();
	wifiMacHelper.SetType("ns3::AdhocWifiMac");

//	Ptr<UniformRandomVariable> randomizer = CreateObject<UniformRandomVariable>();
//	randomizer->SetAttribute("Min", DoubleValue(10));
//	randomizer->SetAttribute("Max", DoubleValue(100));

	int m_totalNodes = 100;
	NodeContainer nodes;
	nodes.Create(m_totalNodes);

	// 1. Install Wifi
	NetDeviceContainer wifiNetDevices = wifi.Install(wifiPhyHelper, wifiMacHelper, nodes);
	wifiPhyHelper.EnablePcap("wave-simple-80211a", wifiNetDevices);

	MobilityHelper mobility;
	Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();

	int m_totalRows = 10;
	int m_totalColumns = 10;
	int m_distance = 125;
	int m_width = 0;
	int m_height = 0;
	for(int i=1; i<=m_totalRows; i++){
		for(int j=1; j<=m_totalColumns; j++){
			positionAlloc->Add(Vector(m_height, m_width, 0.0));
			m_width += m_distance;
		}
		m_height += m_distance;
		m_width = 0;
	}
	mobility.SetPositionAllocator(positionAlloc);
	mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");

	// 2. Install Mobility model
	mobility.Install(nodes);

	// 3. Install NDN stack
	NS_LOG_INFO("Installing NDN stack");
	ndn::StackHelper ndnHelper;
	ndnHelper.SetOldContentStore("ns3::ndn::cs::Lru", "MaxSize", "1000");
	ndnHelper.SetDefaultRoutes(true);
	ndnHelper.Install(nodes);

	// Set BestRoute strategy
	ndn::StrategyChoiceHelper::Install(nodes, "/", "/localhost/nfd/strategy/best-route");

	// 4. Set up applications
	NS_LOG_INFO("Installing Applications");

	ndn::AppHelper consumerHelper1("ns3::ndn::ConsumerCbr");
	consumerHelper1.SetPrefix("/pair-1/");
	consumerHelper1.SetAttribute("Frequency", DoubleValue(1.0)); // no. of interests per second
	consumerHelper1.Install(nodes.Get(2));

	ndn::AppHelper producerHelper1("ns3::ndn::Producer");
	producerHelper1.SetPrefix("/pair-1/");
	producerHelper1.SetAttribute("PayloadSize", StringValue("1200"));
	producerHelper1.Install(nodes.Get(12));


	ndn::AppHelper consumerHelper2("ns3::ndn::ConsumerCbr");
	consumerHelper2.SetPrefix("/pair-2/");
	consumerHelper2.SetAttribute("Frequency", DoubleValue(1.0)); // no. of interests per second
	consumerHelper2.Install(nodes.Get(7));

	ndn::AppHelper producerHelper2("ns3::ndn::Producer");
	producerHelper2.SetPrefix("/pair-2/");
	producerHelper2.SetAttribute("PayloadSize", StringValue("1200"));
	producerHelper2.Install(nodes.Get(17));


	ndn::AppHelper consumerHelper3("ns3::ndn::ConsumerCbr");
	consumerHelper3.SetPrefix("/pair-3/");
	consumerHelper3.SetAttribute("Frequency", DoubleValue(1.0)); // no. of interests per second
	consumerHelper3.Install(nodes.Get(71));

	ndn::AppHelper producerHelper3("ns3::ndn::Producer");
	producerHelper3.SetPrefix("/pair-3/");
	producerHelper3.SetAttribute("PayloadSize", StringValue("1200"));
	producerHelper3.Install(nodes.Get(81));


	ndn::AppHelper consumerHelper4("ns3::ndn::ConsumerCbr");
	consumerHelper4.SetPrefix("/pair-4/");
	consumerHelper4.SetAttribute("Frequency", DoubleValue(1.0)); // no. of interests per second
	consumerHelper4.Install(nodes.Get(68));

	ndn::AppHelper producerHelper4("ns3::ndn::Producer");
	producerHelper4.SetPrefix("/pair-4/");
	producerHelper4.SetAttribute("PayloadSize", StringValue("1200"));
	producerHelper4.Install(nodes.Get(78));


	ndn::AppHelper consumerHelper5("ns3::ndn::ConsumerCbr");
	consumerHelper5.SetPrefix("/pair-5/");
	consumerHelper5.SetAttribute("Frequency", DoubleValue(1.0)); // no. of interests per second
	consumerHelper5.Install(nodes.Get(89));

	ndn::AppHelper producerHelper5("ns3::ndn::Producer");
	producerHelper5.SetPrefix("/pair-5/");
	producerHelper5.SetAttribute("PayloadSize", StringValue("1200"));
	producerHelper5.Install(nodes.Get(99));

	ndn::AppHelper consumerHelper6("ns3::ndn::ConsumerCbr");
	consumerHelper6.SetPrefix("/pair-6/");
	consumerHelper6.SetAttribute("Frequency", DoubleValue(1.0)); // no. of interests per second
	consumerHelper6.Install(nodes.Get(32));

	ndn::AppHelper producerHelper6("ns3::ndn::Producer");
	producerHelper6.SetPrefix("/pair-6/");
	producerHelper6.SetAttribute("PayloadSize", StringValue("1200"));
	producerHelper6.Install(nodes.Get(43));


	// 5. Running simulation
	Simulator::Stop(Seconds(20));

	ndn::AppDelayTracer::InstallAll("app-delays-trace.txt");
	ndn::CsTracer::InstallAll("cs-trace.txt", Seconds(1));
	ndn::L3RateTracer::InstallAll("rate-trace.txt", Seconds(1.0));
	L2RateTracer::InstallAll("drop-trace.txt", Seconds(0.5));

	Simulator::Run();
	Simulator::Destroy();

	return 0;
}

}

int main(int argc, char* argv[]) {
	return ns3::main(argc, argv);
}
