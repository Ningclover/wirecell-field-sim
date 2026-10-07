import numpy as np
import subprocess
import random

# --- CONFIGURATION ---
#param_init = [-0.03, 2.00, 1.00, 1.75, 600]  # Initial parameter values
param_init = [-0.04, 2.3, 1.2, 2.50, 640]  # Initial parameter values
step_sizes = [0.01, 0.1, 0.1, 0.1, 10]      # Step sizes for each parameter
bounds = [(-0.05, -0.00), (1.90, 2.50), (1.00, 1.20), (2.30, 2.50), (600, 650)]  # Parameter limits
num_iterations = 50  # Number of MCMC iterations

# Function to run shell script with given parameters
def run_shell_script(params):
    cmd = ["./run_sim_formcmc_dev.sh"] + [str(p) for p in params]
    result = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    # Capture and display standard output
    #if result.stdout:
    #    print(f"Shell script output:\n{result.stdout.strip()}")

    # Capture and display errors, if any
    if result.stderr:
        print(f"Shell script error:\n{result.stderr.strip()}")
        return False  # Indicate that an error occurred

    return True  # Indicate success

# Function to run ROOT script and extract chi2 value
def get_chi2():
    # cmd = ["root", "-q", "-b", "compute_chi2.C"]
    # result = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    
    # Extract chi2 value from ROOT output
    try:
        with open("chi2_output.txt", "r") as f:
            return float(f.readline().strip())  # Read chi2 value
    except Exception as e:
        print(f"Error reading chi2_output.txt: {e}")
        return None

# Function to generate new parameter set within bounds
def propose_new_params(current_params):
    new_params = []
    for i, param in enumerate(current_params):
        step = step_sizes[i] if random.random() < 0.5 else -step_sizes[i]
        new_param = param + step
        
        # Ensure the parameter stays within bounds
        lower, upper = bounds[i]
        new_param = max(lower, min(new_param, upper))
        
        new_params.append(new_param)
    
    return new_params

# --- MCMC Optimization ---
current_params = param_init
current_chi2 = None
print(current_params)
# Run first evaluation
run_shell_script(current_params)
print("run end")
current_chi2 = get_chi2()
if current_chi2 is None:
    raise RuntimeError("Failed to retrieve chi2 from ROOT script.")

best_params = current_params[:]
best_chi2 = current_chi2

for i in range(num_iterations):
    new_params = propose_new_params(current_params)
    
    print(new_params)
    # Run simulation with new parameters
    run_shell_script(new_params)
    print("run end")
    new_chi2 = get_chi2()
    print("chi2 = ",new_chi2)

    if new_chi2 is None:
        continue  # Skip this iteration if chi2 retrieval fails

    # Metropolis-Hastings acceptance rule
    if new_chi2 < current_chi2 or np.exp((current_chi2 - new_chi2) / 2) > random.random():
        current_params = new_params
        current_chi2 = new_chi2

    # Track the best parameters found
    if current_chi2 < best_chi2:
        best_params = current_params[:]
        best_chi2 = current_chi2

    print(f"Iteration {i+1}/{num_iterations}: Chi2 = {current_chi2}, Params = {current_params}")

print("\nOptimization Complete!")
print(f"Best parameters: {best_params}")
print(f"Best Chi2: {best_chi2}")
