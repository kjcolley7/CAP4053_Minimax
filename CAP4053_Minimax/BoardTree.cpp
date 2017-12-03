//
//  BoardTree.cpp
//  CAP4053_Minimax
//
//  Created by Charlos Staszeski on 12/2/17.
//  Copyright © 2017 kTeam. All rights reserved.
//

#include "BoardTree.h"
#include <iostream>


const unsigned BoardTree::kMaximumDepth = 5;


BoardTree::BoardTree(Board initBoard)
: mHead(ShiftNode::allocate(initBoard)), mBestMove(Direction::UP) {
	for(int i = 0; i < 4; i++) {
		ThreadInfo* info = &mThreads[i];
		info->dir = (Direction)i;
		info->mutex.lock();
		info->thread = std::make_unique<std::thread>([info] {
			while(!info->terminate) {
				info->mutex.lock();
				info->result = info->start->getMinScore(kMaximumDepth - 1);
				info->mutex.unlock();
			}
		});
	}
}

BoardTree::~BoardTree() {
	for(int i = 0; i < 4; i++) {
		mThreads[i].terminate = true;
		mThreads[i].mutex.unlock();
	}
	
	for(int i = 0; i < 4; i++) {
		mThreads[i].thread->join();
	}
}

void BoardTree::setBoard(Board newBoard) {
	mHead->setBoard(newBoard);
}


bool BoardTree::isValid() const {
	return mHead != nullptr;
}


Direction BoardTree::getBestMove() {
	// Give the threads arguments and unlock their mutexes to start them
	for(int i = 0; i < 4; i++) {
		mThreads[i].start = mHead->getChild(mThreads[i].dir);
		if(mThreads[i].start) {
			mThreads[i].mutex.unlock();
		}
	}
	
	// Lock the mutexes to wait for each thread to finish, and collect the best move
	int score = INT_MIN;
	for(int i = 0; i < 4; i++) {
		if(!mThreads[i].start) {
			continue;
		}
		
		mThreads[i].mutex.lock();
		if(mThreads[i].result > score) {
			score = mThreads[i].result;
			mBestMove = mThreads[i].dir;
		}
	}
	
	// Pick the direction of a valid move in cases where we fail to choose correctly
	if(score == INT_MIN) {
		for(int i = 0; i < 4; i++) {
			if(mThreads[i].start) {
				mBestMove = mThreads[i].dir;
			}
		}
	}
	
	char cDir;
	switch(mBestMove) {
		case Direction::UP:
			cDir = 'U';
			break;
		
		case Direction::DOWN:
			cDir = 'D';
			break;
		
		case Direction::LEFT:
			cDir = 'L';
			break;
		
		case Direction::RIGHT:
			cDir = 'R';
			break;
	}
	
	std::cerr << "Picking direction " << cDir << " with score " << score << std::endl;
	return mBestMove;
}


void BoardTree::placedTile(unsigned row, unsigned col, Tile tile) {
	PlaceNode* firstMove = mHead->getChild(mBestMove);
	if(firstMove) {
		updateHead(firstMove->getChild(row, col, tile));
	}
	else {
		updateHead(nullptr);
	}
}


void BoardTree::updateHead(ShiftNode* newHead) {
	mHead->prune(newHead);
	mHead->deallocate();
	mHead = newHead;
}
