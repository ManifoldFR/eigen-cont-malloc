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
def __(bench_data):
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
    processed_ = [extract(d) for d in bench_data]
    processed_
    return extract, processed_, re


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
def __():
    lss = {1: "-", 2: "dotted", 4: "-.", 6: "--"}
    colors = {
        "basic": "tab:blue",
        "contigs": "tab:orange",
        "veccontigs": "tab:green"
    }
    return colors, lss


@app.cell
def __(colors, df, lss, plt):
    _fig, _ax = plt.subplots(figsize=(8,6))

    for (_name, _th), _sdf in df.partition_by(["b", "threads"], as_dict=True).items():
        _ax.plot(_sdf["N"], _sdf["scaled"],
                 ls=lss[_th],
                 color=colors[_name],
                 label="{} ({}c)".format(_name, _th))

    _ax.legend()
    _ax.set_ylabel("Time $\\times$ threads [µs]")
    _ax.set_xlabel("$N$")
    # _ax.set_yscale("log")
    # _ax.set_xscale("log")
    _ax.set_title("Scaled time vs. workload (lower is better)")
    _fig.savefig("time-vs-size-vs-threads.png")
    _ax
    return


@app.cell
def __(df, pl):
    _grp = df.filter(pl.col("threads") == 1).drop("threads").group_by(["b", "N"])
    _divs = _grp.first().to_dicts()
    _divs = { (d["b"], d["N"]) : d["time"] for d in _divs }
    print(_divs)
    def fn(entry):
        return _divs[(entry[0], entry[1])] / entry[4]

    df_eff = df.with_columns(df.map_rows(fn).rename({"map": "eff"}))
    return df_eff, fn


@app.cell
def __(colors, df_eff, lss, plt):
    _fig, _ax = plt.subplots(figsize=(10,8))


    for (name, th), _sdf in df_eff.partition_by(["b", "threads"], as_dict=True).items():
        if th == 1: continue
        _ax.plot(_sdf["N"], _sdf["eff"],
                 ls=lss[th],
                 color=colors[name],
                 label="{} ({}c)".format(name, th))

    _ax.legend()
    _ax.set_ylabel("Efficiency")
    _ax.set_xlabel("$N$")
    _ax.set_xscale("log")
    _ax.set_title("Efficiency vs. workload vs. threads")
    _ax
    return name, th


if __name__ == "__main__":
    app.run()
