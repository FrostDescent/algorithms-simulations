<?php
	ini_set('memory_limit', '-1');

	define("MCGAP_PROFIT_FILE", "mcgap_input/profit.txt");
	define("MCGAP_WEIGHT_FILE", "mcgap_input/weight.txt");
	define("MCGAP_SIZES_FILE", "mcgap_input/sizes.txt");
	define("MCGAP_NUM_CLASSES_FILE", "mcgap_input/num_classes.txt");
	define("MCGAP_NUM_ITEMS_PER_CLASS_FILE", "mcgap_input/num_items_per_class.txt");

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

	//create profit and weight files
	echo "Generating profit and weight files..\n";
	$fprofit = fopen(MCGAP_PROFIT_FILE, "w");
	$fweight = fopen(MCGAP_WEIGHT_FILE, "w");
	
	for($switch = 0 ; $switch < $numOfSwitches ; ++$switch){		
		for($i = 0 ; $i < $numOfFlows ; ++$i){
			set_time_limit(30);
			
			$flow = $flows[$i];
			$flowSize = $flowSizes[$i];
			if(in_array($switch, $flow)){
				for($j = 0 ; $j < $numOfItemsInClass ; ++$j){
					fwrite($fprofit, $samplingRatesProfits[$i][$j].",");
					fwrite($fweight, ceil($samplingRates[$i][$j] * $flowSize).",");
				}
			}else{
				for($j = 0 ; $j < $numOfItemsInClass ; ++$j){
					fwrite($fprofit, "0,");
					fwrite($fweight, "0,");
				}
			}
		}
	}
	
	fclose($fprofit);
	fclose($fweight);
	
	//create knapsack sizes array
	$fsizes = fopen(MCGAP_SIZES_FILE, "w");
	for($i = 0 ; $i < $numOfSwitches ; $i++){
		fwrite($fsizes, $knapsackSize.",");
	}
	fclose($fsizes);
	
	//create num_classes file
	$fp = fopen(MCGAP_NUM_CLASSES_FILE, "w");
	fwrite($fp, count($flows));
	fclose($fp);
	
	//create num_items_per_class file
	$fp = fopen(MCGAP_NUM_ITEMS_PER_CLASS_FILE, "w");
	fwrite($fp, count($samplingRates[0]));
	fclose($fp);

 	echo "Done generating files.\n";
	echo "Solving MC-GAP..\n";

	$mcgap = `../mcgap`;


	//create flow->switch and flow->rate arrays
	$mcgapArr = explode("|", $mcgap);
	$totalProfit = $mcgapArr[0];
	$allocation = explode(",", $mcgapArr[1]);

	echo "Done Solving MC-GAP.\n";
	echo "Profit: $totalProfit\n";

	$flowSwitch = array();
	$flowRate = array();

	for($flow = 0 ; $flow < $numOfFlows ; ++$flow){
		for($rate = 0 ; $rate < $numOfItemsInClass ; ++$rate){
			if($allocation[$flow * $numOfItemsInClass + $rate] >= 0){
				$flowSwitch[$flow] = $allocation[$flow * $numOfItemsInClass + $rate];
				$flowRate[$flow] = $samplingRates[$flow][$rate];
			}
		}		
	}

	$OUT = array();
	$OUT["flowSwitch"] = $flowSwitch;
	$OUT["flowRate"] = $flowRate;
	$OUT["profit"] = $totalProfit;
	file_put_contents("mcgap_allocation.txt", serialize($OUT));
?>
