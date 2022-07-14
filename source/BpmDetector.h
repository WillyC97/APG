/*
 * Copyright (C) 2006-2021  Music Technology Group - Universitat Pompeu Fabra
 *
 * This file is part of Essentia
 *
 * Essentia is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License as published by the Free
 * Software Foundation (FSF), either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the Affero GNU General Public License
 * version 3 along with this program.  If not, see http://www.gnu.org/licenses/
 */

#include <iostream>
#include "algorithmfactory.h"
#include "streaming/algorithms/poolstorage.h"
#include "scheduler/network.h"

using namespace essentia;
using namespace essentia::streaming;
using namespace essentia::scheduler;

class BpmDetector
{
public :
    void CalculateBpm(std::string audioFilename)
    {
        // register the algorithms in the factory(ies)
        essentia::init();

        Pool pool;

        std::cout << "Rhythm extractor (beat tracker, BPM, positions of tempo changes) based on multifeature beat tracker (see the BeatTrackerMultiFeature algorithm)" << std::endl;

        streaming::AlgorithmFactory& factory = streaming::AlgorithmFactory::instance();

        Algorithm* monoloader = factory.create("MonoLoader",
                                             "filename", audioFilename,
                                             "sampleRate", 44100.);
        // using 'multifeature' method for best accuracy,
        // but it requires the largest computation time
        Algorithm* rhythmextractor = factory.create("RhythmExtractor2013",
                                                  "method", "multifeature");


        /////////// CONNECTING THE ALGORITHMS ////////////////
        std::cout << "-------- connecting algos --------" << std::endl;

        monoloader->output("audio")             >> rhythmextractor->input("signal");
        rhythmextractor->output("ticks")        >> PC(pool, "rhythm.ticks");
        rhythmextractor->output("confidence")   >> PC(pool, "rhythm.ticks_confidence");
        rhythmextractor->output("bpm")          >> PC(pool, "rhythm.bpm");
        rhythmextractor->output("estimates")    >> PC(pool, "rhythm.estimates");
        rhythmextractor->output("bpmIntervals") >> PC(pool, "rhythm.bpmIntervals");

        /////////// STARTING THE ALGORITHMS //////////////////
        std::cout << "-------- start processing " << audioFilename << " --------" << std::endl;

        Network network(monoloader);
        network.run();

        // printing results
        std::cout << "-------- results --------" << std::endl;
        std::cout << "bpm: " << pool.value<Real>("rhythm.bpm") << std::endl;
        std::cout << "ticks detection confidence: " << pool.value<Real>("rhythm.ticks_confidence") << std::endl;

        essentia::shutdown();
    }
};
