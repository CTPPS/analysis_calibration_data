#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/FWLite/interface/ChainEvent.h"
#include "DataFormats/Common/interface/DetSetVector.h"

#include "DataFormats/CTPPSReco/interface/CTPPSLocalTrackLite.h"

#include "alignment.h"
#include "fill_info.h"
#include "proton_reconstruction.h"

#include <vector>
#include <string>

using namespace std;
using namespace edm;

//----------------------------------------------------------------------------------------------------

int main()
{
	// get input
	vector<string> input_files = {
		"/afs/cern.ch/user/j/jkaspar/public/ctpps/event_pick/pickevents_B.root",
		"/afs/cern.ch/user/j/jkaspar/public/ctpps/event_pick/pickevents_C.root",
		"/afs/cern.ch/user/j/jkaspar/public/ctpps/event_pick/pickevents_G.root",
	};
	fwlite::ChainEvent event(input_files);

	//  load alignment collection
	AlignmentResultsCollection alignmentCollection;
	alignmentCollection.Load("alignment/collect_alignments.out");

	// load fill-alignment mapping
	InitFillInfoCollection();

	// load optical functions
	InitReconstruction();

	// loop over the chain entries
	AlignmentResults *alignments = NULL;
	unsigned int prev_run = 0;

	for (event.toBegin(); ! event.atEnd(); ++event)
	{
		printf("\nevent %u:%llu\n", event.id().run(), event.id().event());

		// get track data
		fwlite::Handle< vector<CTPPSLocalTrackLite> > hTracks;
		hTracks.getByLabel(event, "ctppsLocalTrackLiteProducer");

		// get alignment
		if (prev_run != event.id().run() || alignments == NULL)
		{
			const auto &fillInfo = fillInfoCollection.FindByRun(event.id().run());
			prev_run = event.id().run();

			const auto alignment_it = alignmentCollection.find(fillInfo.alignmentTag);
			if (alignment_it == alignmentCollection.end())
			{
				printf("ERROR: no alignment for tag '%s'.\n", fillInfo.alignmentTag.c_str());
				return 1;
			}

			//printf("INFO: loaded alignment with tag '%s'.\n", fillInfo.alignmentTag.c_str());

			alignments = &alignment_it->second;
		}

		// apply alignment
		auto tracksAligned = alignments->Apply(*hTracks);

		// proton reconstruction, RP by RP
		for (const auto track : tracksAligned)
		{
			ProtonData proton;
			ReconstructProtonFromOneRP(track, proton);

			if (proton.valid)
			{
				TotemRPDetId rpId(track.getRPId());
				unsigned int rpDecId = rpId.arm()*100 + rpId.station()*10 + rpId.rp();

				printf("    RP %u : xi = %.3f +- %.4f\n", rpDecId, proton.xi, proton.xi_unc);
			}
		}
	}

	// clean up
	return 0;
}
