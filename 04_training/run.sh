model="sf0"
size=17

echo "./dataset ../trace/${model} ${model} < ${model}_fine"
echo python3 train_baseline.py cluster_${model}10_${size}/
echo python3 train_hashlayer_gh.py model/model_${model}10_4096_2048_1e-05.torchsave cluster_${model}10_${size}/ 128 1 0.001 0.05
echo python3 model_converter_gh.py model/model_hash_${model}10_128_4096_2048_1_0.001.torchsave cluster_${model}10_${size}/
