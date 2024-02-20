import marimo

__generated_with = "0.2.4"
app = marimo.App()


@app.cell
def __():
    import orjson
    import pandas as pd
    import numpy as np
    import matplotlib.pyplot as plt
    return np, orjson, pd, plt


@app.cell
def __(orjson):
    with open("data/parallel.json") as f:
        bench_data = orjson.loads(f.read())

    bench_data = bench_data["benchmarks"]
    return bench_data, f


@app.cell
def __(bench_data):
    bench_data
    return


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
def __(pd, processed_):
    df = pd.DataFrame.from_dict(processed_)
    df.set_index(["b", "threads", "N"], inplace=True)
    df
    return df,


@app.cell
def __(df):
    dfc_ = df.loc["contigs"].reset_index(level=1)
    dfc_
    return dfc_,


@app.cell
def __(dfc_, plt):
    fig, ax = plt.subplots()
    for name, group in dfc_.groupby("threads"):
        group.plot(x="N", y="time", ax=ax, label="{:d} threads".format(name))
    ax.set_title("Time (contig. struct)")
    ax

    return ax, fig, group, name


if __name__ == "__main__":
    app.run()
