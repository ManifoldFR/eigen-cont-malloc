import marimo

__generated_with = "0.2.5"
app = marimo.App(width="full")


@app.cell
def __():
    import orjson
    import polars as pl
    import numpy as np
    import matplotlib.pyplot as plt
    return np, orjson, pl, plt


@app.cell
def __():
    import hvplot.polars

    hvplot.extension('matplotlib')
    return hvplot,


@app.cell
def __(orjson):
    with open("data/parallel.json") as f:
        bench_data = orjson.loads(f.read())

    bench_data = bench_data["benchmarks"]
    bench_data
    return bench_data, f


@app.cell
def __():
    import re

    def extract(entry):
        name = entry["name"]
        ex = re.compile("_([a-z]*)/N:([0-9]*)/threads:([0-9]*)")
        m = ex.search(name)
        b, N, th = m.groups()
        return {
            "b": b,
            "N": int(N),
            "threads": int(th),
            "time": entry["real_time"]
        }
    return extract, re


@app.cell
def __(bench_data, extract):
    processed_ = [extract(d) for d in bench_data]
    processed_
    return processed_,


@app.cell
def __(pl, processed_):
    df = pl.from_dicts(processed_)
    df = df.with_columns((pl.col("time") * pl.col("threads")).alias("scaled"))
    df
    return df,


@app.cell
def __(df, pl, plt):
    _dfc = df.filter(pl.col("threads") == 2).drop("threads")
    _fig, _ax = plt.subplots()

    for (_name,), _sdf in _dfc.partition_by(["b"], as_dict=True).items():
        _ax.plot(_sdf["N"], _sdf["time"], label=_name)

    _ax.legend()
    _ax.set_ylabel("time")
    _ax.set_xlabel("N")
    _ax
    return


@app.cell
def __(df, pl):
    _grp = df.filter(pl.col("threads") == 1).group_by(["b", "N"])
    _grp.first()
    return


@app.cell
def __(df, plt):
    _fig, _ax = plt.subplots(figsize=(8,6))

    lss = {1: "-", 2:"dotted", 4:"-.", 6:"--"}
    colors = {
        "basic": "tab:blue",
        "contigs": "tab:orange",
        "veccontigs": "tab:green"
    }

    for (name,th), _sdf in df.partition_by(["b", "threads"], as_dict=True).items():
        _ax.plot(_sdf["N"], _sdf["scaled"],
                 ls=lss[th], color=colors[name],
                 label="{} ({}c)".format(name, th))

    _ax.legend()
    _ax.set_ylabel("Time $\\times$ threads [µs]")
    _ax.set_xlabel("$N$")
    _ax.set_yscale("log")
    _ax.set_xscale("log")
    _ax.set_title("Scaled time vs. workload vs. threads")
    _fig.savefig("time-vs-size-vs-threads.png")
    _ax
    return colors, lss, name, th


if __name__ == "__main__":
    app.run()
