import argparse
import subprocess
import csv
from itertools import product
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path
from datetime import datetime

def run_benchmark(lambda_val, tau, benchmark_bin):
    """运行基准测试程序并返回第二行CSV数据"""
    cmd = [
        str(benchmark_bin),
        str(lambda_val),
        str(tau)
    ]
    
    try:
        result = subprocess.run(
            cmd, 
            check=True,
            capture_output=True,
            text=True,
            timeout=3000  # 50分钟超时
        )
        return result.stdout
    except subprocess.CalledProcessError as e:
        print(f"Error running {' '.join(cmd)}:\n{e.stderr}")
        return None
    except Exception as e:
        print(f"Unexpected error with {' '.join(cmd)}: {str(e)}")
        return None

def main():
    parser = argparse.ArgumentParser(description='One-Tree-Sampler参数空间搜索工具')

    # 获取当前时间并格式化为字符串（精确到分钟）
    current_time = datetime.now().strftime("%Y-%m-%d-%H-%M")
    parser.add_argument('--output', '-o', default=f'param_results_{current_time}.csv',
                      help='输出CSV文件路径')
    parser.add_argument('--benchmark-bin', '-b', 
                      default='./build/my_app',
                      help='基准测试程序路径')
    parser.add_argument('--threads', '-j', type=int, default=4,
                      help='并行工作线程数')
    
    args = parser.parse_args()
    
    # 定义参数空间
    lambdas = [40]
    taus = range(10, 11) # 0-20, 21个值
    
    # 准备CSV文件
    csv_path = Path(args.output)
    csv_path.parent.mkdir(parents=True, exist_ok=True)
    
    with csv_path.open('w', newline='') as f:
        writer = csv.writer(f)
        # 写入表头
        writer.writerow([
            'lambda', 'tau', 't_open_1_8', 't_open_1_4', 't_open_1_2'
        ])
    
    # 生成所有参数组合
    param_combinations = []
    for csp in lambdas:
        for tau in taus:
            param_combinations.append((csp, tau))

    # param_combinations = product(lambdas, taus, w_grinds, rej_diffs)
    # 使用线程池并行执行
    with ThreadPoolExecutor(max_workers=args.threads) as executor:
        futures = []
        for lambda_val, tau in param_combinations:
            futures.append(
                executor.submit(
                    run_benchmark,
                    lambda_val,
                    tau,
                    Path(args.benchmark_bin).absolute()
                )
            )
        
        # 收集结果并写入CSV
        with csv_path.open('a', newline='') as f:  # 追加模式
            writer = csv.writer(f)
            for future in futures:
                result = future.result()
                if result:
                    writer.writerow(result.strip().split(','))
                    f.flush()  # 实时写入

if __name__ == "__main__":
    main()
