#!/bin/sh

# The experiment script starts 11 experiments with the
# following configurations and stores their results in
# seperate folders:
#
#                 |              No.
# Options         | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13
# alpha=0.7       |   |   |   |   | x |   |   |   |   |    |    |    |   
# alpha=0.8       | x | x | x | x |   |   | x | x | x |  x |  x |  x |  x
# alpha=0.9       |   |   |   |   |   | x |   |   |   |    |    |    |   
# epsilon=0.1     | x | x | x | x | x | x |   | x | x |  x |  x |  x |  x
# epsilon=0.2     |   |   |   |   |   |   | x |   |   |    |    |    |   
# 10k episod.     | x | x | x |   |   |   |   |   |   |    |    |    |   
# 25k episod.     |   |   |   | x | x | x | x | x | x |  x |  x |  x |  x
# -10 reward      | x |   |   |   |   |   |   |   |   |    |    |    |   
# -100 reward     |   | x |   |   |   |   |   |   |   |    |    |    |   
# -1000 reward    |   |   | x | x | x | x | x | X | X |  x |  x |  x |  x
# w/o pseudo-rew. | x | x | x | x | x | x | x |   |   |    |  x |  x |  x
# w/ pseudo-rew.  |   |   |   |   |   |   |   | x | x |  x |    |    |   
# seq. root       | x | x | x | x | x | x | x | x | x |    |    |  x |  x
# dyn. root       |   |   |   |   |   |   |   |   |   |  x |  x |    |   
# static speed    | x | x | x | x | x | x | x | x |   |    |    |  x |  x
# dyn. speed      |   |   |   |   |   |   |   |   | x |  x |  x |    |   
# static gear     | x | x | x | x | x | x | x | x | x |  x |    |  x |   
# dyn. gear       |   |   |   |   |   |   |   |   |   |    |  x |    |  x
# static steer    |   |   |   |   |   |   |   |   |   |    |    |    |   
# from scratch    | x | x | x | x | x | x | x | x |   |    |    |  x |  x
# prelearned      |   |   |   |   |   |   |   |   | x |  x |  x |    |   

# Because the latter experiments depend on the results of the former,
# the suite is devided into several sections (see below).
# The script needs two arguments, the section to start and the one to end.

# The default agent compilation is:
#    make episodes=10000 alpha=0.8 epsilon=0.1 abstractReward=-10.0 pseudoRew=0 staticRoot=1 staticSpeed=1 staticGear=1

echo "--------------------------------------------"
echo "-------- HRLDriver experiment suite --------"
echo "--------------------------------------------\n\n"

if [ $# -lt 2 ]
  then
    echo "Error: No experiment start and end section provided.\n\n"
    echo "Usage: $0 startSectionNo endSectionNo\n"
  else
    STARTSEC=$1
    ENDSEC=$2
    echo "---- Starting experiments ----\n"

    # Create a data folder to store results in
    DATE=`date +%Y-%m-%d`
    RESULTDIR="experiments-$DATE"
    mkdir $RESULTDIR
    CURRSEC=$STARTSEC
    while [ $CURRSEC -le $ENDSEC ]
    do
      case "$CURRSEC" in
        1)  echo "---- Experiment 1 ----\n"
            # Compile the agent with the specific params
            echo "- Building the agent..."
            make clean >/dev/null 2>&1
            make >/dev/null 2>&1
            echo "- Starting agent..."
            # Run the experiment
            ./client mode:1 expFilePath:""
            # Back up the experience and stat files
            mkdir "$RESULTDIR/e1"
            yes | cp -rf "./data/." "./$RESULTDIR/e1"
            echo "- Results stored. Done.\n\n"
            # END experiment 1 #

            echo "---- Experiment 2 ----\n"
            # Compile the agent with the specific params
            echo "- Building the agent..."
            make clean >/dev/null 2>&1
            make abstractReward=-100.0 >/dev/null 2>&1
            echo "- Starting agent..."
            # Run the experiment
            ./client mode:1 expFilePath:""
            # Back up the experience and stat files
            mkdir "$RESULTDIR/e2"
            yes | cp -rf "./data/." "./$RESULTDIR/e2"
            echo "- Results stored. Done.\n\n"
            # END experiment 2 #

            echo "---- Experiment 3 ----\n"
            # Compile the agent with the specific params
            echo "- Building the agent..."
            make clean >/dev/null 2>&1
            make abstractReward=-1000.0 >/dev/null 2>&1
            echo "- Starting agent..."
            # Run the experiment
            ./client mode:1 expFilePath:""
            # Back up the experience and stat files
            mkdir "$RESULTDIR/e3"
            yes | cp -rf "./data/." "./$RESULTDIR/e3"
            echo "- Results stored. Done.\n\n"
            # END experiment 3 #
            ;;
        2)  echo "---- Experiment 4 ----\n"
            # Compile the agent with the specific params
            echo "- Building the agent..."
            make clean >/dev/null 2>&1
            make episodes=25000 abstractReward=-1000.0 >/dev/null 2>&1
            echo "- Starting agent..."
            # Run the experiment
            ./client mode:1 expFilePath:""
            # Back up the experience and stat files
            mkdir "$RESULTDIR/e4"
            yes | cp -rf "./data/." "./$RESULTDIR/e4"
            echo "- Results stored. Done.\n\n"
            # END experiment 4 #

            echo "---- Experiment 5 ----\n"
            # Compile the agent with the specific params
            echo "- Building the agent..."
            make clean >/dev/null 2>&1
            make episodes=25000 alpha=0.7 abstractReward=-1000.0 >/dev/null 2>&1
            echo "- Starting agent..."
            # Run the experiment
            ./client mode:1 expFilePath:""
            # Back up the experience and stat files
            mkdir "$RESULTDIR/e5"
            yes | cp -rf "./data/." "./$RESULTDIR/e5"
            echo "- Results stored. Done.\n\n"
            # END experiment 5 #

            echo "---- Experiment 6 ----\n"
            # Compile the agent with the specific params
            echo "- Building the agent..."
            make clean >/dev/null 2>&1
            make episodes=25000 alpha=0.9 abstractReward=-1000.0 >/dev/null 2>&1
            echo "- Starting agent..."
            # Run the experiment
            ./client mode:1 expFilePath:""
            # Back up the experience and stat files
            mkdir "$RESULTDIR/e6"
            yes | cp -rf "./data/." "./$RESULTDIR/e6"
            echo "- Results stored. Done.\n\n"
            # END experiment 6 #

            echo "---- Experiment 7 ----\n"
            # Compile the agent with the specific params
            echo "- Building the agent..."
            make clean >/dev/null 2>&1
            make episodes=25000 epsilon=0.2 abstractReward=-1000.0 >/dev/null 2>&1
            echo "- Starting agent..."
            # Run the experiment
            ./client mode:1 expFilePath:""
            # Back up the experience and stat files
            mkdir "$RESULTDIR/e7"
            yes | cp -rf "./data/." "./$RESULTDIR/e7"
            echo "- Results stored. Done.\n\n"
            # END experiment 7 #
	          ;;
        3)  echo "---- Experiment 8 ----\n"
            # Compile the agent with the specific params
            echo "- Building the agent..."
            make clean >/dev/null 2>&1
            make episodes=25000 abstractReward=-1000.0 pseudoRew=1 >/dev/null 2>&1
            echo "- Starting agent..."
            # Run the experiment
            ./client mode:1 expFilePath:""
            # Back up the experience and stat files
            mkdir "$RESULTDIR/e8"
            yes | cp -rf "./data/." "./$RESULTDIR/e8"
            echo "- Results stored. Done.\n\n"
            # END experiment 8 #
            ;;
        4)  echo "---- Experiment 9 ----\n"
            # Compile the agent with the specific params
            echo "- Building the agent..."
            make clean >/dev/null 2>&1
            make episodes=25000 abstractReward=-1000.0 pseudoRew=1 staticSpeed=0 >/dev/null 2>&1
            echo "- Starting agent..."
            # Run the experiment
            ./client mode:1 expFilePath:"data/HRLDriverData.xml"
            # Back up the experience and stat files
            mkdir "$RESULTDIR/e9"
            yes | cp -rf "./data/." "./$RESULTDIR/e9"
            echo "- Results stored. Done.\n\n"
            # END experiment 9 #

            echo "---- Experiment 10 ----\n"
            # Compile the agent with the specific params
            echo "- Building the agent..."
            make clean >/dev/null 2>&1
            make episodes=25000 abstractReward=-1000.0 pseudoRew=1 staticSpeed=0 staticGear=0 >/dev/null 2>&1
            echo "- Starting agent..."
            # Run the experiment
            ./client mode:1 expFilePath:"data/HRLDriverData.xml"
            # Back up the experience and stat files
            mkdir "$RESULTDIR/e10"
            yes | cp -rf "./data/." "./$RESULTDIR/e10"
            echo "- Results stored. Done.\n\n"
            # END experiment 10 #

            echo "---- Experiment 11 ----\n"
            # Compile the agent with the specific params
            echo "- Building the agent..."
            make clean >/dev/null 2>&1
            make episodes=25000 abstractReward=-1000.0 pseudoRew=1 staticRoot=0 staticSpeed=0 staticGear=0 >/dev/null 2>&1
            echo "- Starting agent..."
            # Run the experiment
            ./client mode:1 expFilePath:"data/HRLDriverData.xml"
            # Back up the experience and stat files
            mkdir "$RESULTDIR/e11"
            yes | cp -rf "./data/." "./$RESULTDIR/e11"
            echo "- Results stored. Done.\n\n"
            # END experiment 11 #
            ;;
	5)  echo "---- Experiment 12 ----\n"
            # Compile the agent with the specific params
            echo "- Building the agent..."
            make clean >/dev/null 2>&1
            make episodes=25000 abstractReward=-1000.0 staticSteer=1 staticSpeed=0 >/dev/null 2>&1
            echo "- Starting agent..."
            # Run the experiment
            ./client mode:1 expFilePath:""
            # Back up the experience and stat files
            mkdir "$RESULTDIR/e12"
            yes | cp -rf "./data/." "./$RESULTDIR/e12"
            echo "- Results stored. Done.\n\n"
            # END experiment 12 #

	    echo "---- Experiment 13 ----\n"
            # Compile the agent with the specific params
            echo "- Building the agent..."
            make clean >/dev/null 2>&1
            make episodes=25000 abstractReward=-1000.0 staticSteer=1 staticGear=0 >/dev/null 2>&1
            echo "- Starting agent..."
            # Run the experiment
            ./client mode:1 expFilePath:""
            # Back up the experience and stat files
            mkdir "$RESULTDIR/e13"
            yes | cp -rf "./data/." "./$RESULTDIR/e13"
            echo "- Results stored. Done.\n\n"
            # END experiment 13 #
	    ;;
      esac
      CURRSEC=$((CURRSEC + 1))
    done
    echo "---- All experiments done. ----\n"
fi
