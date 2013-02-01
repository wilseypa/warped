// See copyright notice in file Copyright in the root directory of this archive.

/*============================================================*/
/* Future implementations                                     */
/* ----------------------                                     */
/*  1. Top array structure replaced by vector. Need to ensure */
/*     its length remains within limit                        */
/*                                                            */
/*  2. Need to bucket underflow in the rungs                  */
/*============================================================*/

#ifndef LadderQueue_H_
#define LadderQueue_H_

#include <iostream>
#include <set>

using std::multiset;

using namespace std;

#define THRESHOLD       1000
#define MAX_TOP_NUM     3000
#define MAX_RUNG_NUM      10
#define MAX_BUCKET_NUM  1000

class LadderQueue {

private:

	/* Top variables */
	const Event*  top[MAX_TOP_NUM];
	//vector<const Event*> top;
	unsigned int  maxTS;
	unsigned int  minTS;
	unsigned int  nTop;
	unsigned int  topStart;

	/* Rungs */
	const Event*  rung[MAX_RUNG_NUM][MAX_BUCKET_NUM][THRESHOLD];
	unsigned int  bucketWidth[MAX_RUNG_NUM];
	unsigned int  nBucket[MAX_RUNG_NUM][MAX_BUCKET_NUM];
	unsigned int  numBucket[MAX_RUNG_NUM];
	unsigned int  nRung;
	unsigned int  rStart[MAX_RUNG_NUM];
	unsigned int  rCur[MAX_RUNG_NUM];

	/* Bottom */
	multiset<const Event*, receiveTimeLessThanEventIdLessThan> bottom;
	unsigned int nBot;
	unsigned int botMinTS;

	/* Create (here implicitly allocate) a new rung */
	bool create_new_rung(unsigned int numEvents, bool *isBucketWidthStatic, unsigned int initRange)
	{
		unsigned int bucketIndex = 0;
		*isBucketWidthStatic = false;

		/* Check event count */
		if ( !numEvents ) {
			cout << "Invalid event count received for new rung creation." << endl;
			return false;
		}

		/* Check overflow of no of rungs */
		if(nRung == MAX_RUNG_NUM)
		{
			cout << "Sorry, I ran out of rungs." << endl;
			return false;
		}

		/* Check if rungs are available for allocation */
		if( !numEvents ) {
			cout << "Sorry, received event count as 0." << endl;
			return false;
		}

		/* Check if rungs already exist */
		if(nRung > 0) {

			if( nRung >= MAX_RUNG_NUM ) {
				cout << "Max limit of rungs reached." << endl;
				return false;
			}
			bucketWidth[nRung] =
				(bucketWidth[nRung-1] + numEvents-1) / numEvents;
			if( (bucketWidth[nRung] == bucketWidth[nRung-1]) || (bucketWidth[nRung] == 0) ) {
				cout << "Reached smallest possible bucketWidth for rungs. No further rungs needed." << endl;
				*isBucketWidthStatic = true;
				return true;
			}
			rStart[nRung] = rCur[nRung] = initRange;
			numBucket[nRung] = 0;

			for(bucketIndex = 0; bucketIndex < MAX_BUCKET_NUM; bucketIndex++) {
				nBucket[nRung][bucketIndex] = 0;
			}

		} else { /* If first rung is getting created */
			bucketWidth[0] = (maxTS - minTS + nTop -1 )/ nTop;

			if( bucketWidth[0] == 0  ) {
				*isBucketWidthStatic = true;
				return true;
			}

			rStart[0] = rCur[0] = initRange;
			topStart = maxTS;
			numBucket[0] = 0;

			for(bucketIndex = 0; bucketIndex < MAX_BUCKET_NUM; bucketIndex++) {
				nBucket[0][bucketIndex] = 0;
			}
		}
		nRung++;
		//cout << "Rung " << nRung << ": maxTS, minTS, nTop, bucketWidth = " 
		//	<< maxTS << "," << minTS << "," << nTop << "," << bucketWidth[nRung-1] << endl;

		return true;
	}

	/* Recurse rung */
	unsigned int recurse_rung() {

		unsigned int bucketIndex = 0, tempIndex = 0, newBucketIndex = 0;
		bool isFindBucket = false;
		bool isBucketWidthStatic = false;

		do {
			isFindBucket = false;

			if(nRung == 0) {
				cout << "No rungs available to check." << endl;
				return MAX_BUCKET_NUM;
			}

			bucketIndex = 0;
			/* find the next non-empty bucket from the lowest rung */
			rCur[nRung-1] = rStart[nRung-1];
			while( (bucketIndex < numBucket[nRung-1]) && (nBucket[nRung-1][bucketIndex] == 0) ) {
				bucketIndex++;
				rCur[nRung-1] += bucketWidth[nRung-1];
			}

			/* if rung is empty */
			if(bucketIndex == numBucket[nRung-1]) {
				while( (nRung > 0) && (numBucket[nRung-1] == 0) ) {
					numBucket[nRung-1] = rStart[nRung-1] = rCur[nRung-1] = bucketWidth[nRung-1] = 0;
					nRung--;
				}
				isFindBucket = true;
			}

			/* Check if number of events in bucket is >= threshold */
			if( (!isFindBucket) && (bucketIndex < numBucket[nRung-1]) && (nBucket[nRung-1][bucketIndex] >= THRESHOLD) ) {
				if( !create_new_rung( nBucket[nRung-1][bucketIndex], &isBucketWidthStatic, botMinTS ) ) {
					cout << "Failed to create the required rung." << endl;
					return MAX_BUCKET_NUM;
				}

				/* Recopy events from bucket to a new rung */
				for(tempIndex = 0; tempIndex < nBucket[nRung-2][bucketIndex]; tempIndex++) {

					newBucketIndex =
						( rung[nRung-2][bucketIndex][tempIndex]->getReceiveTime().getApproximateIntTime() - 
							rStart[nRung-1] ) / 
								bucketWidth[nRung-1];

					if( (newBucketIndex >= MAX_BUCKET_NUM) || (nBucket[nRung-1][newBucketIndex] >= THRESHOLD) ) {
						cout << "5.Ran out of bucket space." << endl;
						return MAX_BUCKET_NUM;
					}

					if(numBucket[nRung-1] < newBucketIndex+1) {
						numBucket[nRung-1] = newBucketIndex+1;
					}
					rung[nRung-1][newBucketIndex][ nBucket[nRung-1][newBucketIndex]++ ] = rung[nRung-2][bucketIndex][tempIndex];
				}

				nBucket[nRung-2][bucketIndex] = 0;
				if(numBucket[nRung-2] == bucketIndex+1) {
					numBucket[nRung-2]   = 0;
					/* caution: rung deletions cannot happen for intermediate rungs */
					//rStart[nRung-2]      = 0;
					//rCur[nRung-2]        = 0;
					//bucketWidth[nRung-2] = 0;
				}
				isFindBucket = true;
			}
		} while (isFindBucket);

		return bucketIndex;
	}

public:

	LadderQueue() {

	}

	~LadderQueue() {

	}

	const Event* begin() {

		unsigned int bucketIndex = 0, tempIndex = 0;
		bool isBucketWidthStatic = false;

		/* Remove from bottom if not empty */
		if ( !bottom.empty() ) {
			return *bottom.begin();
		}
		nBot = 0; /* extra precaution when bottom is empty */
		botMinTS = 0;

		/* If rungs exits, remove from rungs */
		if (nRung > 0) {
			if( MAX_BUCKET_NUM <= (bucketIndex = recurse_rung()) )
			{
				cout << "1. Received invalid Bucket index." << endl;
				return NULL;
			}

			for(tempIndex = 0; tempIndex < nBucket[nRung-1][bucketIndex]; tempIndex++) {
				bottom.insert(rung[nRung-1][bucketIndex][tempIndex]);
				nBot     = (unsigned int) bottom.size();
				botMinTS = (*bottom.begin())->getReceiveTime().getApproximateIntTime();
			}
			nBucket[nRung-1][bucketIndex] = 0;

			/* If bucket returned is the last valid rung of the bucket */
			if ( numBucket[nRung-1] == bucketIndex+1) {

				numBucket[nRung-1] = rStart[nRung-1] = rCur[nRung-1] = bucketWidth[nRung-1] = 0;
				while( (nRung > 0) && (numBucket[nRung-1] == 0) ) {
					nRung--;
				}
			}

			return *bottom.begin();
		}

		/* Move from top to top of empty ladder */
		/* Check if failed to create the first rung */
		if( !create_new_rung(nTop, &isBucketWidthStatic, minTS)) {
			cout << "Failed to create the required rung." << endl;
			return NULL;
		}

		/* Transfer events from Top to 1st rung of Ladder */
		for(tempIndex = 0; tempIndex < nTop; tempIndex++) {
			bucketIndex = 
				(unsigned int) (top[tempIndex]->getReceiveTime().getApproximateIntTime() -
							rStart[0]) / bucketWidth[0];

			if( (bucketIndex >= MAX_BUCKET_NUM) || (nBucket[0][bucketIndex] >= THRESHOLD) ) {
				cout << "2:Ran out of bucket space." << endl;
				return NULL;
			}

			if( numBucket[0] < bucketIndex+1 ) {
				numBucket[0] = bucketIndex+1;
			}

			rung[0][bucketIndex][ nBucket[0][bucketIndex]++ ] = top[tempIndex];
		}
		nTop = 0;

		/* Copy events from bucket_k into Bottom */
		if( MAX_BUCKET_NUM <= (bucketIndex = recurse_rung()) )
		{
			cout << "2. Received invalid Bucket index." << endl;
			return NULL;
		}

		for(tempIndex = 0; tempIndex < nBucket[0][bucketIndex]; tempIndex++) {
			bottom.insert(rung[0][bucketIndex][tempIndex]);
			nBot     = (unsigned int) bottom.size();
			botMinTS = (*bottom.begin())->getReceiveTime().getApproximateIntTime();
		}
		nBucket[0][bucketIndex] = 0;

		/* If bucket returned is the last valid rung of the bucket */
		if ( numBucket[0] == bucketIndex+1) {

			numBucket[0] = rStart[0] = rCur[0] = bucketWidth[0] = 0;
			while( (nRung > 0) && (numBucket[nRung-1] == 0) ) {
				nRung--;
			}
		}

		return *bottom.begin();
	}

	void clear() {

		unsigned int rungIndex = 0, bucketIndex = 0;

		/* Top variables */
		maxTS = minTS = nTop = topStart = 0;

		/* Rungs */
		for(rungIndex = 0; rungIndex < MAX_RUNG_NUM; rungIndex++) {
			bucketWidth[rungIndex] = rStart[rungIndex] = rCur[rungIndex] = numBucket[rungIndex]   = 0;

			for(bucketIndex = 0; bucketIndex < MAX_BUCKET_NUM; bucketIndex++) {
				nBucket[rungIndex][bucketIndex] = 0;
			}
		}
		nRung = 0;

		/* Purge bottom */
		bottom.clear();
		nBot = 0;
		botMinTS = 0;
	}

	const Event* dequeue() {
		const Event *retVal = begin();

		if(retVal) {
			bottom.erase(bottom.begin());
		}
		if(bottom.empty()) {
			nBot = 0;
			botMinTS = 0;
		} else {
			botMinTS = (*bottom.begin())->getReceiveTime().getApproximateIntTime();
			nBot     = (unsigned int) bottom.size();
		}

		return retVal;
	}

	bool empty() {

		bool isEmpty = false;
		if( (nRung == 0) && (nTop == 0) && (bottom.empty())) {
			isEmpty = true;
			nBot = 0; /* precaution */
			botMinTS = 0; /* precaution */
		}

		return isEmpty;
	}

	const Event* end() {
		return NULL;
	}

	void erase(const Event *delEvent) {

		/* Check whether valid event received */
		if( !delEvent) {
			cout << "Invalid event erase request received." << endl;
			return;
		}

		unsigned int rungIndex = 0, bucketIndex = 0, shiftIndex = 0, eleIndex = 0, checkBIndex = 0;
		/* Check top */
		for(int eleIndex = 0; eleIndex < nTop; eleIndex++) {
			if( top[eleIndex] == delEvent ) {
				for (shiftIndex = eleIndex+1; shiftIndex < nTop; shiftIndex++) {
					top[shiftIndex-1] = top[shiftIndex];
				}
				nTop--;
				return;;
			}
		}

		/* Check rungs */
		for (rungIndex = 0; rungIndex < nRung; rungIndex++) {
			checkBIndex = 0;
			for (bucketIndex = 0; bucketIndex < numBucket[rungIndex]; bucketIndex++) {
				for(eleIndex = 0; eleIndex < nBucket[rungIndex][bucketIndex]; eleIndex++) {
					if( rung[rungIndex][bucketIndex][eleIndex] == delEvent ) {
						for (shiftIndex = eleIndex+1; shiftIndex < nBucket[rungIndex][bucketIndex]; shiftIndex++) {
							rung[rungIndex][bucketIndex][shiftIndex-1] = rung[rungIndex][bucketIndex][shiftIndex];
						}
						nBucket[rungIndex][bucketIndex]--;

						/* If the only non-empty bucket of the rung goes empty */
						if( (nBucket[rungIndex][bucketIndex] ==0) && (numBucket[rungIndex] == bucketIndex+1) ) {
							numBucket[rungIndex] = checkBIndex;
							if( (nRung == rungIndex+1) ) {
								while( (nRung > 0) && (numBucket[nRung-1] == 0) ) {
									nRung--;
								}
							}
						}
						return;
					}
					checkBIndex = bucketIndex+1;
				}
			}
		}

		/* Precaution */
		while( (nRung > 0) && (numBucket[nRung-1] == 0) ) {
			nRung--;
		}

		if( !bottom.empty() ) {
			bottom.erase(delEvent);
		}
		if( !bottom.empty() ) {
			botMinTS = (*bottom.begin())->getReceiveTime().getApproximateIntTime();
			nBot     = (unsigned int) bottom.size();
		} else {
			nBot = 0; /* precaution */
			botMinTS = 0; /* precaution */
		}
	}

	const Event* insert(const Event *newEvent) {

		bool isBucketWidthStatic = false;

		/* Check whether valid event received */
		if( !newEvent) {
			cout << "Invalid event insertion request received." << endl;
			return NULL;
		}

		/* Insert into TOP */
		if( nTop >= MAX_TOP_NUM ) {
			cout << "Reached max capacity of Top." << endl;
			return NULL;
		}

		if( newEvent->getReceiveTime().getApproximateIntTime() >= topStart ) {
			if(minTS > newEvent->getReceiveTime().getApproximateIntTime()) {
				minTS = newEvent->getReceiveTime().getApproximateIntTime();
			}
			if(maxTS < newEvent->getReceiveTime().getApproximateIntTime()) {
				maxTS = newEvent->getReceiveTime().getApproximateIntTime();
			}

			top[nTop++] = newEvent;

			if(!newEvent) {
				cout << "NULL1" << endl;
			}
			return newEvent;
		}

		/* Step through rungs */
		unsigned int rungIndex = 0;
		unsigned int bucketIndex = 0;

		while( (rungIndex < nRung) && (newEvent->getReceiveTime().getApproximateIntTime() < rCur[rungIndex]) ) {
			rungIndex++;
		}

		if( rungIndex < nRung ) { /* found a rung */
			bucketIndex = 
				(unsigned int) (newEvent->getReceiveTime().getApproximateIntTime() -
							rStart[rungIndex]) / bucketWidth[rungIndex];

			if( (bucketIndex >= MAX_BUCKET_NUM) || (nBucket[rungIndex][bucketIndex] >= THRESHOLD) ) {
				cout << "3:Ran out of bucket space." << endl;
				return NULL;
			}

			if( numBucket[rungIndex] < bucketIndex+1 ) {
				numBucket[rungIndex] = bucketIndex+1;
			}

			rung[rungIndex][bucketIndex][nBucket[rungIndex][bucketIndex]++] = newEvent;

			if(!newEvent) {
				cout << "NULL2" << endl;
			}
			return newEvent;

		} else { /* If rung not found */
			if(nBot >= THRESHOLD) {
				/* Check if failed to create a rung */
				if( !create_new_rung(nBot, &isBucketWidthStatic, botMinTS)) {
					cout << "Failed to create the required rung." << endl;
					return NULL;
				}

				/* Intentionally let the bottom continue to overflow */
				if( isBucketWidthStatic ) {
					bottom.insert(newEvent);
					nBot     = (unsigned int) bottom.size();
					botMinTS = (*bottom.begin())->getReceiveTime().getApproximateIntTime();

					if(!newEvent) {
						cout << "NULL3" << endl;
					}
					return newEvent;
				}

				/* Transfer bottom to new rung */
				const Event *puiEvent = NULL;
				multiset<const Event*, receiveTimeLessThanEventIdLessThan>::iterator mIterate;
				unsigned int bottomIndex = 0;

				for(mIterate = bottom.begin(); mIterate != bottom.end(); mIterate++) {
					bottomIndex++;
					puiEvent = *bottom.begin();

					if( puiEvent->getReceiveTime().getApproximateIntTime() < rStart[nRung-1] ) {
						cout << "Not in the right place(" << puiEvent->getReceiveTime().getApproximateIntTime() 
							<< "," << rStart[nRung-1] << ")" << endl;
					}

					bucketIndex = 
						(unsigned int) ((puiEvent->getReceiveTime().getApproximateIntTime() -
									rStart[nRung-1]) / bucketWidth[nRung-1]);

					if( (bucketIndex >= MAX_BUCKET_NUM) || (nBucket[nRung-1][bucketIndex] >= THRESHOLD) ) {
						cout << "1:Ran out of bucket space." << bucketIndex+1 << endl;
						return NULL;
					}

					if( numBucket[nRung-1] < bucketIndex+1 ) {
						numBucket[nRung-1] = bucketIndex+1;
					}

					rung[nRung-1][bucketIndex][ nBucket[nRung-1][bucketIndex]++ ] = puiEvent;
					bottom.erase(mIterate);
				}
				nBot = 0;
				botMinTS = 0;

				/* Insert new element in the new and populated rung */
				bucketIndex = 
					(unsigned int) ((newEvent->getReceiveTime().getApproximateIntTime() -
								rStart[nRung-1]) / bucketWidth[nRung-1]);

				if( (bucketIndex >= MAX_BUCKET_NUM) || (nBucket[nRung-1][bucketIndex] >= THRESHOLD) ) {
					cout << "4.Ran out of bucket space." << endl;
					return NULL;
				}

				if( numBucket[nRung-1] < bucketIndex+1 ) {
					numBucket[nRung-1] = bucketIndex+1;
				}

				rung[nRung-1][bucketIndex][ nBucket[nRung-1][bucketIndex]++ ] = newEvent;

				if(!newEvent) {
					cout << "NULL4" << endl;
				}
				return newEvent;

			} else { /* If BOTTOM is within threshold */
				bottom.insert(newEvent);
				nBot     = (unsigned int) bottom.size();
				botMinTS = (*bottom.begin())->getReceiveTime().getApproximateIntTime();

				if(!newEvent) {
					cout << "NULL5" << endl;
				}
				return newEvent;
			}
		}
	}



};

#endif /* LadderQueue_H_ */
