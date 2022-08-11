# Usage

## 01_bruteforce
- brute-force search
> make
> ./bf [file_name] [num_thread]

## 02_baseline
> make
> 
> ./lsh_inf_comp [input_file] [window_size] [SF_NUM] [FEATURE_NUM]
> 
> ./random_inf_comp [input_file]
> 
> ./recent_inf_comp [input_file]
> 
> ./sota_inf_comp [input_file]

## 03_clustering
> ./coarse [input_file] [num_thread] > [coarse_output]
> 
> ./fine [input_file] [num_thread] < [coarse_output] > [fine_output]

## 04_training
> python train_baseline.py cluster_@@_@@@@@
> 
> python3 train_hashlayer_gh.py model/model.4096_2048_1e-05.torchsave cluster_@@_@@@@@ 128 1 0.001 0.05
> 
> python3 model_converter_gh.py model/model_hash.128_4096_2048_1_0.002_0.05.torchsave

## 05_infer
> ./build.sh
> 
> ./ann_inf_comp [input_file] [script_module] [threshold] // ex)./ann_inf_comp dedup ../04_training/model/model_hash_small_128_4096_2048_1_0.001.torchsave.pt 128

infer를 수행할 때에는 libtorch init이 필요하므로, 기존에 했던 deepsketch build 방식대로 수행하면 된다.
