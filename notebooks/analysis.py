import marimo

__generated_with = "0.2.2"
app = marimo.App()


@app.cell
def __():
    import re
    import pandas as pd
    import orjson
    return orjson, pd, re


@app.cell
def __(orjson):
    with open("notebooks/par.json") as f:
        data = orjson.loads(f.read())

    bench_data = data["benchmarks"]
    return bench_data, data, f


@app.cell
def __(bench_data, pd, re):
    regexpr = re.compile(r"(openmp|tbb)/N:([0-9]*)/threads:([0-9]*)")

    def get_run_param(entry):
        n, N, nt = regexpr.findall(entry["name"])[0]
        return n, int(N), int(nt)

    processed_data = {
        get_run_param(e): e["real_time"] for e in bench_data
    }
    series = pd.Series(processed_data)
    return get_run_param, processed_data, regexpr, series


@app.cell
def __(series):

    df = series['openmp'].unstack(1)
    df
    return df,


@app.cell
def __():
    import numpy as np
    import seaborn as sns

    sns.set_style("darkgrid")
    return np, sns


@app.cell
def __(df):
    ax = df.plot.bar()
    ax.set_xlabel("$N$")
    ax.set_ylabel("Time (µs)")
    ax
    return ax,


@app.cell
def __(df):
    df_normalized = df.copy()
    procs = df.columns.values
    _scale = df_normalized.iloc[:, 0].values[:, None]
    df_normalized = _scale / (df_normalized * procs)
    df_normalized
    return df_normalized, procs


@app.cell
def __(df_normalized):
    ax2 = df_normalized.plot.bar()
    ax2.set_title("Efficiency $ e = t_1/(Pt_P)$")
    ax2.set_xlabel("$N$")
    ax2
    return ax2,


@app.cell
def __(ax2):
    fig = ax2.get_figure()
    fig.savefig("efficiency_noncontig_malloc2.pdf")

    return fig,


if __name__ == "__main__":
    app.run()
