import marimo

__generated_with = "0.2.4"
app = marimo.App()


@app.cell
def __():
    import re
    import pandas as pd
    import orjson
    import matplotlib.pyplot as plt
    return orjson, pd, plt, re


@app.cell
def __(orjson):
    filename = "par_nomove.json"
    with open(f"{filename}") as f:
        data = orjson.loads(f.read())

    bench_data = data["benchmarks"]
    return bench_data, data, f, filename


@app.cell
def __(bench_data, pd, re):
    regexpr = re.compile(r"_(openmp|contigs|veccontigs)/N:([0-9]*)/threads:([0-9]*)")

    def get_run_param(entry):
        n, N, nt = regexpr.findall(entry["name"])[0]
        return n, int(N), int(nt)

    def is_mean_aggreg(entry):
        return entry["run_type"] == "aggregate" and entry["aggregate_name"] == "mean"

    processed_data = {
        get_run_param(e): e["real_time"] for e in bench_data
        # if is_mean_aggreg(e)
    }
    series = pd.Series(processed_data)
    return get_run_param, is_mean_aggreg, processed_data, regexpr, series


@app.cell
def __(series):
    df = series.unstack(2)
    df
    return df,


@app.cell
def __(df):
    import numpy as np
    import seaborn as sns

    sns.set_style("darkgrid")

    ax = df.xs(150, level=1).plot.bar()
    # ax = df.plot.barh()
    ax.set_xlabel("$N$")
    ax.set_ylabel("Time (µs)")
    ax.get_figure().tight_layout()
    ax
    return ax, np, sns


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
    ax2 = df_normalized.loc['openmp'].plot.bar()
    ax2.set_title("Efficiency $ e = t_1/(Pt_P)$ (no contig)")
    ax2.set_xlabel("$N$")
    ax2.get_figure().tight_layout()
    ax2
    return ax2,


@app.cell
def __(df_normalized):
    ax3 = df_normalized.loc['contigs'].plot.bar()
    ax3.set_title("Efficiency $ e = t_1/(Pt_P)$ (Contig)")
    ax3.set_xlabel("$N$")
    ax3.get_figure().tight_layout()
    ax3
    return ax3,


@app.cell
def __(df_normalized):
    ax4 = df_normalized.loc['veccontigs'].plot.bar()
    ax4.set_title("Efficiency $ e = t_1/(Pt_P)$ (Vec. Contig)")
    ax4.set_xlabel("$N$")
    ax4.get_figure().tight_layout()
    ax4
    return ax4,


@app.cell
def __(ax2):
    fig = ax2.get_figure()
    # fig.savefig(f"efficiency_{filename}.png")
    return fig,


if __name__ == "__main__":
    app.run()
