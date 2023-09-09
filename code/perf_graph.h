/*
File:   perf_graph.h
Author: Taylor Robbins
Date:   09\09\2023
*/

#ifndef _PERF_GRAPH_H
#define _PERF_GRAPH_H

#define PERF_GRAPH_WIDTH           120  //at 30fps, that's 4 seconds
#define PERF_TICK_WIDTH            2    //px (total = 240px)
#define PERF_GRAPH_DISPLAY_HEIGHT  50   //px

struct PerfGraph_t
{
	bool enabled;
	bool paused;
	reci mainRec;
	r32 values[PERF_GRAPH_WIDTH]; //elapsedMs
};

#endif //  _PERF_GRAPH_H
