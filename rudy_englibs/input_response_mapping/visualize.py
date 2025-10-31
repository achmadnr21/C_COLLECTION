import pandas as pd
import matplotlib.pyplot as plt

def visualize_response_mapping(csv_file):
    # Baca CSV
    df = pd.read_csv(csv_file)

    # Pastikan kolom ada
    if "input" not in df.columns or "response" not in df.columns:
        raise ValueError("CSV harus punya kolom 'input' dan 'response'")

    # Ambil data
    input_data = df["input"].values
    response_data = df["response"].values

    # Plot
    plt.figure(figsize=(10, 6))
    plt.plot(input_data, response_data, label="Response Mapping", color="blue")
    plt.title("Input-Response Mapping")
    plt.xlabel("Input")
    plt.ylabel("Response")
    plt.grid(True)
    plt.legend()
    plt.show()

if __name__ == "__main__":
    visualize_response_mapping("response.csv")
