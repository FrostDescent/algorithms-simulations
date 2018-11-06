<?php

	ini_set('memory_limit', '-1');

	if(isset($argv[1])){
		define("LOAD", $argv[1]);
	}else{
		die("No load supplied.");
	}

	define("SWITCH_CAPACITY", 1000);
	define("NUM_OF_ITERATIONS", 1500);	
	define("FLOWS_CHANGED_IN_ITERATION", 10);
	define("NUM_OF_INITIAL_FLOWS", 800);
	define("FLOWS_FILE", "flows_100_nodes_16k_brite.csv");				
	
	echo "Creating point for load ".LOAD.PHP_EOL;
	
	$flowSamplingRates = array(0, 1/250, 1/500, 1/1000, 1/2000);
	

	function clearEOL($str){
		$str = str_replace(PHP_EOL, "", $str);
		$str = str_replace("\n", "", $str);
		$str = str_replace("\r", "", $str);

		return $str;
	}

	function uniqueRand($n, $min = 0, $max = null){
		if($max === null)
			$max = getrandmax();
		$array = range($min, $max);
		$return = array();
		$keys = array_rand($array, $n);
		if($n == 1)
			$keys = array($keys);			
		foreach($keys as $key)
			$return[] = $array[$key];
		return $return;
	}

	function generateNumbers($sum, $count){
		if($count == 1){
			return array($sum);
		}

		$delimiters = uniqueRand($count-1, 1, $sum-1);
		sort($delimiters);

		$ret = array();		
		$ret[] = $delimiters[0];
		for($i = 1 ; $i < count($delimiters) ; $i++){
			$ret[] = $delimiters[$i] - $delimiters[$i-1];
		}
		
		$ret[] = $sum - $delimiters[count($delimiters)-1];

		return $ret;
	}
	
	function mlog($str){
		$fh = fopen('log/point_'.LOAD.'.txt', 'a') or die("can't open file");
		fwrite($fh, $str.PHP_EOL);
		fclose($fh);
	}
	
	function generateProfitFunction($samplingRates){
		$profitsMin = mt_rand(1, 1000);
		$profitsMax = mt_rand($profitsMin, 1000);
								
		$temp = array(0);
		$tempMax = $profitsMax;
		for($j = 0 ; $j < count($samplingRates)-1 ; ++$j){
			$newProfit = mt_rand($profitsMin, $tempMax);
			$temp[] = $newProfit;
			$tempMax = $newProfit;
		}
		
		return $temp;
	}

	function logInstance($instance){
		static $instanceNum = 1;
			
		$fh = fopen('log/point_'.LOAD.'_instance_'.$instanceNum.'.txt', 'w') or die("can't open file");
		fwrite($fh, "Num of switches: ".$instance['num_switches'].PHP_EOL);
		fwrite($fh, "Sampling Rates: ".print_r($instance['sampling_rates'], true).PHP_EOL);
		fwrite($fh, "Sampling Rates Profits: ".print_r($instance['sampling_rates_profits'], true).PHP_EOL);		
		fwrite($fh, "Switch Capacity: ".$instance['switches_capacity'].PHP_EOL);
		fwrite($fh, "Flows: ".print_r($instance['flows'], true).PHP_EOL);
		fwrite($fh, "Flows Sizes: ".print_r($instance['flows_sizes'], true).PHP_EOL);
		fwrite($fh, "Flows Sizes Sum: ".array_sum($instance['flows_sizes']).PHP_EOL);																
		
		fclose($fh);
		
		file_put_contents('log/point_'.LOAD.'_instance_'.$instanceNum.'.inst', serialize($instance));
		
		$instanceNum++;
	}

	function generateInstance($flows){
		global $flowSamplingRates;
		
		mlog("Generating instance..");
		
		//create array of switches
		$switches = array();
		foreach($flows as $flow){
			$flow = clearEOL($flow);
			$flowArr = explode(";", $flow);
			foreach($flowArr as $switch){

				if(in_array($switch, $switches) == false){
					$switches[] = $switch;
				}
			}
		}

		//change the names of switches
		$newFlows = array();
		foreach($flows as $flow){
			$flow = clearEOL($flow);

			$flowArr = explode(";", $flow);
			$newFlowArr = array();
			foreach($flowArr as $switch){
				$index = array_search($switch, $switches);
				$newFlowArr[] = $index;
			}
			$newFlows[] = $newFlowArr;
		}


		##############
			$numOfSwitches = count($switches);
			$numOfFlows = count($newFlows);

			$samplingRates = array();
			$samplingRatesProfits = array();
		
			for($i = 0 ; $i < $numOfFlows ; ++$i){
						
				$samplingRates[] = $flowSamplingRates;
				
				$samplingRatesProfits[] = generateProfitFunction($flowSamplingRates);

			}
		
			$knapsackSize = SWITCH_CAPACITY;

		##############
		
		//Generate flows sizes		
		$maxLoad = ($knapsackSize*$numOfSwitches) / $samplingRates[0][count($samplingRates[0])-1];
		$flowSizes = generateNumbers(floor($maxLoad*LOAD), $numOfFlows);


		$OUT['num_switches'] = $numOfSwitches;
		$OUT['sampling_rates'] = $samplingRates;
		$OUT['sampling_rates_profits'] = $samplingRatesProfits;
		$OUT['switches_capacity'] = $knapsackSize;
		$OUT['flows'] = $newFlows;
		$OUT['flows_sizes'] = $flowSizes;

		//save instance to file
		file_put_contents("instance.txt", serialize($OUT));
		
		logInstance($OUT);
	}	
	
	function updateInstance($currentFlows, $newInsertedFlows){
		global $flowSamplingRates;
		
		$removedFlowsIndices = array_rand($currentFlows, count($newInsertedFlows));
		if(count($newInsertedFlows) == 1){
			$removedFlowsIndices = array($removedFlowsIndices);
		}
		
		//Replace the removed flow with the new flows
		for($i = 0 ; $i < count($newInsertedFlows) ; $i++){
			$currentFlows[$removedFlowsIndices[$i]] = $newInsertedFlows[$i];
		}
		
		$flows = $currentFlows;
		
		//create array of switches
		$switches = array();
		foreach($flows as $flow){
			$flow = clearEOL($flow);
			$flowArr = explode(";", $flow);
			foreach($flowArr as $switch){

				if(in_array($switch, $switches) == false){
					$switches[] = $switch;
				}
			}
		}

		//change the names of switches
		$newFlows = array();
		foreach($flows as $flow){
			$flow = clearEOL($flow);

			$flowArr = explode(";", $flow);
			$newFlowArr = array();
			foreach($flowArr as $switch){
				$index = array_search($switch, $switches);
				$newFlowArr[] = $index;
			}
			$newFlows[] = $newFlowArr;
		}

		$instance = unserialize(file_get_contents("instance.txt"));
		
		$samplingRatesProfits = $instance["sampling_rates_profits"];
		//update the profits
		for($i = 0 ; $i < count($removedFlowsIndices) ; $i++){
			$samplingRatesProfits[$removedFlowsIndices[$i]] = generateProfitFunction($flowSamplingRates);
		}
		
		$flowSizes = $instance["flows_sizes"];
		//update the flow sizes
		$sumRemovedSizes = 0;
		for($i = 0 ; $i < count($removedFlowsIndices) ; $i++){
			$sumRemovedSizes += $flowSizes[$removedFlowsIndices[$i]];
		}
		
		$newFlowsSizes = generateNumbers($sumRemovedSizes, count($removedFlowsIndices));
		for($i = 0 ; $i < count($removedFlowsIndices) ; $i++){
			$flowSizes[$removedFlowsIndices[$i]] = $newFlowsSizes[$i];
		}
		
		$OUT['num_switches'] = count($switches);
		$OUT['sampling_rates'] = $instance["sampling_rates"];
		$OUT['sampling_rates_profits'] = $samplingRatesProfits;
		$OUT['switches_capacity'] = $instance["switches_capacity"];
		$OUT['flows'] = $newFlows;
		$OUT['flows_sizes'] = $flowSizes;

		//save instance to file
		file_put_contents("instance.txt", serialize($OUT));
		
		logInstance($OUT);
				
		return $currentFlows;
	}

	function getArrayStats($array){
		$out = array();
		$out["min"] = min($array);
		$out["max"] = max($array);
		$out["avg"] = (array_sum($array) / count($array));
		$temp = $array;
		rsort($temp); 
		$middle = round(count($temp) / 2); 
		$out["median"] = $temp[$middle-1];

		return $out;
	}

	#################### Main ##########################

	touch('log/point_'.LOAD.'.txt');
			
	$mcgapProfits = array();
	$onlineProfits = array();

	//Run Initial Instance
	$flows = file(FLOWS_FILE);
	$initialFlows = array_slice($flows, 0, NUM_OF_INITIAL_FLOWS);
	$currentFlowsIndex = NUM_OF_INITIAL_FLOWS;
	
	generateInstance($initialFlows);	
	
	`php run_mcgap.php`;
	`php run_online.php`;

	$mcgap = unserialize(file_get_contents("mcgap_allocation.txt"));
	$online = unserialize(file_get_contents("online_allocation.txt"));

	$mcgapProfits[] = $mcgap["profit"];
	$onlineProfits[] = $online["profit"];	

	$currentFlows = $initialFlows;
	//Run the rest of the instances
	for($i = 1 ; $i < NUM_OF_ITERATIONS ; $i++){	
		echo "Iteration $i".PHP_EOL;			
		$newFlows = array_slice($flows, $currentFlowsIndex, FLOWS_CHANGED_IN_ITERATION);
		$currentFlowsIndex += FLOWS_CHANGED_IN_ITERATION;
		
		$currentFlows = updateInstance($currentFlows, $newFlows); 
		
		`php run_mcgap.php`;
		`php run_online.php`;

		$mcgap = unserialize(file_get_contents("mcgap_allocation.txt"));
		$online = unserialize(file_get_contents("online_allocation.txt"));

		$mcgapProfits[] = $mcgap["profit"];
		$onlineProfits[] = $online["profit"];	
	}

	mlog("MC-GAP Profits: ".print_r($mcgapProfits, true));
	$stats = getArrayStats($mcgapProfits);
	mlog("MC-GAP Profits Min: ".$stats["min"]);
	mlog("MC-GAP Profits Max: ".$stats["max"]);	
	mlog("MC-GAP Profits Avg: ".$stats["avg"]);	
	mlog("MC-GAP Profits Median: ".$stats["median"]);	
	
	mlog("Online Profits: ".print_r($onlineProfits, true));
	$stats = getArrayStats($onlineProfits);
	mlog("MC-GAP Profits Min: ".$stats["min"]);
	mlog("MC-GAP Profits Max: ".$stats["max"]);	
	mlog("MC-GAP Profits Avg: ".$stats["avg"]);	
	mlog("MC-GAP Profits Median: ".$stats["median"]);
?>
