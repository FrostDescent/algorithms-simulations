<?php
	
	ini_set('memory_limit', '-1');

	define("MCKP_PROFIT_FILE", "mckp_input/profit.txt");
	define("MCKP_WEIGHT_FILE", "mckp_input/weight.txt");
	define("MCKP_SIZE_FILE", "mckp_input/size.txt");
	define("MCKP_NUM_CLASSES_FILE", "mckp_input/num_classes.txt");
	define("MCKP_NUM_ITEMS_PER_CLASS_FILE", "mckp_input/num_items_per_class.txt");

	//read instance from file
	$instance = unserialize(file_get_contents("instance.txt"));
	
	$numOfSwitches = $instance['num_switches'];
	$flows = $instance['flows'];
	$numOfFlows = count($flows);
	$flowSizes = $instance['flows_sizes'];
	$samplingRates = $instance['sampling_rates'];
	$samplingRatesProfits = $instance['sampling_rates_profits'];
	$knapsackSize = $instance['switches_capacity'];
	$numOfItemsInClass = count($samplingRates[0]);

	//generate an order for flow arrival
	$flowOrder = range(0, $numOfFlows-1);
	shuffle($flowOrder);

	//create mapping from switches to flows
	$switchFlows = array();

	//create array to store the current profit of each switch
	$switchesProfits = array_fill(0, $numOfSwitches, 0);

	//create array to store the current load on each switch
	$switchLoad = array_fill(0, $numOfSwitches, 0);

	//create array to store which switch samples each flow
	$flowSwitch = array();
	//create array to store which sampling rate each flow is sampled
	$flowRate = array();

	//create knapsack sizes array
	$fsizes = fopen(MCKP_SIZE_FILE, "w");
	fwrite($fsizes, $knapsackSize.",");
	fclose($fsizes);
	
	//create num_items_per_class file
	$fp = fopen(MCKP_NUM_ITEMS_PER_CLASS_FILE, "w");
	fwrite($fp, count($samplingRates[0]));
	fclose($fp);

	$totalProfit = 0;

	for($i = 0 ; $i<$numOfFlows ; ++$i){
		echo $i."/".$numOfFlows."\r";

		$currentFlowId = $flowOrder[$i];
		$currentFlowSwitches = $flows[$currentFlowId];

		//add the flow to the switches
		foreach($currentFlowSwitches as $switch){
			if(array_key_exists($switch, $switchFlows)){
				if(in_array($currentFlowId, $switchFlows[$switch]) == FALSE){
					array_push($switchFlows[$switch], $currentFlowId);
				}
			}else{
				$switchFlows[$switch] = array($currentFlowId);
			}
		}

		//loop through all the switches the new flow traverses
		$bestProfitDelta = -1;
		$bestProfitDeltaSol = array();
		$bestSwitch = -1;
		$bestSwitchFlows = array();
		$bestSwitchLoad = -1;

		foreach($currentFlowSwitches as $switch){
			//get all the flows which pass the current switch
			$currentSwitchFlows = $switchFlows[$switch];

			//remove all the flows which are sampled by other switches			
			$currentSwitchFlows = array_filter($currentSwitchFlows, function($flow) use ($switch) {
																		global $flowSwitch;
																		return array_key_exists($flow, $flowSwitch) == FALSE || 
																			   $flowSwitch[$flow] == $switch;
																	});

			//fix the indices of currentSwitchFlows
			$temp = array();
			foreach($currentSwitchFlows as $flow){
				$temp[] = $flow;
			}
			$currentSwitchFlows = $temp;

			//create array of flow sizes
			$currentSwitchFlowsSizes = array();
			foreach($currentSwitchFlows as $flow){
				$currentSwitchFlowsSizes[] = $flowSizes[$flow];
			}


			//create num_classes file
			$fp = fopen(MCKP_NUM_CLASSES_FILE, "w");
			fwrite($fp, count($currentSwitchFlows));
			fclose($fp);

			//create profit and weight files
			$fprofit = fopen(MCKP_PROFIT_FILE, "w");
			$fweight = fopen(MCKP_WEIGHT_FILE, "w");
			
			///echo "Switch $switch current flows: "; print_r($currentSwitchFlows) ; echo "\n";

			for($j = 0 ; $j < count($currentSwitchFlows) ; ++$j){
				set_time_limit(30);
			
				$flowSize = $currentSwitchFlowsSizes[$j];
				
				for($k = 0 ; $k < $numOfItemsInClass ; ++$k){
					fwrite($fprofit, $samplingRatesProfits[$currentSwitchFlows[$j]][$k].",");
					fwrite($fweight, ceil($samplingRates[$currentSwitchFlows[$j]][$k] * $flowSize).",");
				}
			}

			fclose($fprofit);
			fclose($fweight);

			//run MCKP
			$MCKP = `../RunMCKP`;

			//calculate the profit delta
			$oldProfit = $switchesProfits[$switch];
			$MCKPArr = explode(" ", $MCKP);
			$profit = 0;
			$load = 0;
			for($j = 0 ; $j < count($MCKPArr) ; ++$j){
				if($MCKPArr[$j] > 0){
					$profit += $samplingRatesProfits[$currentSwitchFlows[$j]][$MCKPArr[$j]];
					$load += $currentSwitchFlowsSizes[$j] * $samplingRates[$currentSwitchFlows[$j]][$MCKPArr[$j]];
				}
				
			}
			$profiDelta = $profit - $oldProfit;

			if($profiDelta > $bestProfitDelta){
				$bestProfitDelta = $profiDelta;
				$bestProfitDeltaSol = $MCKPArr;
				$bestSwitch = $switch;
				$bestSwitchFlows = $currentSwitchFlows;
				$bestSwitchLoad = $load;
			}

		}		


		//update the total profit
		$totalProfit += $bestProfitDelta;
		//update where each flow is sampled		
		for($j = 0 ; $j < count($bestProfitDeltaSol) ; ++$j){
			if($bestProfitDeltaSol[$j] > 0){
				$flowSwitch[$bestSwitchFlows[$j]] = $bestSwitch;
				$flowRate[$bestSwitchFlows[$j]] = $samplingRates[$bestSwitchFlows[$j]][$bestProfitDeltaSol[$j]];
			}else{
				//*** We assume here that if a flow rate changed to 0, we can sample it in another switch ***
				unset($flowSwitch[$bestSwitchFlows[$j]]);
				unset($flowRate[$bestSwitchFlows[$j]]);
			}
			
		}
		//update the current profit in the chosen switch
		$switchesProfits[$bestSwitch] += $bestProfitDelta;

	} 
	echo "Total Profit: $totalProfit \n";
	//Note: if sampling the new flow has the same profit as sampling old flow, the MCKP algorithms will choose to sample the new flow

	$OUT = array();
	$OUT["flowSwitch"] = $flowSwitch;
	$OUT["flowRate"] = $flowRate;
	$OUT["profit"] = $totalProfit;
	file_put_contents("online_allocation.txt", serialize($OUT));
?>
