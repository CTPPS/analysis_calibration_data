#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/FWLite/interface/ChainEvent.h"
#include "DataFormats/Common/interface/DetSetVector.h"

#include "DataFormats/CTPPSReco/interface/TotemRPLocalTrack.h"

#include "track_lite.h"
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
		"/afs/cern.ch/user/j/jkaspar/public/event_pick/pickevents_2016B.root",
		"/afs/cern.ch/user/j/jkaspar/public/event_pick/pickevents_2016C.root",
		"/afs/cern.ch/user/j/jkaspar/public/event_pick/pickevents_2016G.root",
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
		fwlite::Handle< DetSetVector<TotemRPLocalTrack> > aodTracks;
		aodTracks.getByLabel(event, "totemRPLocalTrackFitter");

		// produce collection of lite tracks (in future this will be done in miniAOD)
		TrackDataCollection liteTracks;
		for (const auto &ds : *aodTracks)
		{
			const auto &rpId = ds.detId();

			for (const auto &tr : ds)
			{
				liteTracks[rpId] = tr;
			}
		}

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
		TrackDataCollection liteTracksAligned = alignments->Apply(liteTracks);

		// proton reconstruction, RP by RP
		for (const auto it : liteTracksAligned)
		{
			ProtonData proton;
			ReconstructProtonFromOneRP(it.first, it.second, proton);

			if (proton.valid)
				printf("    RP %u : xi = %.3f +- %.3f\n", it.first, proton.xi, proton.xi_unc);
		}
	}

	// clean up
	return 0;
}
