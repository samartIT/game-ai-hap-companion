"""บทที่ 14 — การลดทอนข้อมูลของการทดลอง (ขั้นที่ 3 และ 4 ของระเบียบวิธี) สำหรับไฟล์ CSV ทั้งชุด

อ่านไฟล์ Bench_<คลาส>_N<N>_I<I>_<yyyymmdd>_<hhmmss>.csv ทุกไฟล์ในโฟลเดอร์ CSV
ใช้คอลัมน์ GameThreadTime ของแถวที่เป็นเฟรม (แถวแรก = ชื่อคอลัมน์; แถวของเฟรมจบที่แถวชื่อคอลัมน์ซ้ำ
ซึ่งขึ้นต้นด้วย EVENTS หรือแถวข้อมูลประกอบที่ขึ้นต้นด้วย '[') เรียงค่าแล้วตัดเฟรมร้อยละ 5 ที่ปลายบน
และร้อยละ 5 ที่ปลายล่าง (รวมร้อยละ 10 เท่ากับ =TRIMMEAN(ช่วง, 0.1) ซึ่งอาร์กิวเมนต์ที่สองคือสัดส่วนรวม)
ค่าเฉลี่ยของค่าที่เหลือคือค่าตัวแทนของรอบ ต่อเซลล์ (แบบ, N, I) พิมพ์ค่าเฉลี่ย ± ส่วนเบี่ยงเบนมาตรฐาน
ของค่าตัวแทนจากสามรอบ (STDEV.S, n - 1: ความต่างระหว่างการเปิดโปรแกรมแต่ละครั้ง ไม่ใช่ความแกว่งระหว่างเฟรม)
ค่าที่ใช้ในการอ่านผล และแถวของตารางทั้งสองของบทในรูป LaTeX

วิธีใช้ (Command Prompt ที่โฟลเดอร์ companion/HAPBook/ ต้องมี Python 3):
    python Tools\\ch14_reduce.py [โฟลเดอร์ CSV] [--exclude ชื่อไฟล์.csv ...]
โฟลเดอร์เริ่มต้นคือ Saved/Profiling/CSV ของโปรเจกต์นี้
--exclude ข้ามไฟล์ที่ระบุ (เช่น รอบทดลองด้วยมือ) โดยไม่ลบไฟล์
"""
import csv, glob, math, os, re, statistics, sys

ARM = {'BP_Arm_A_C': 'A', 'BenchmarkActorCpp': 'B', 'BP_Arm_C_C': 'C'}
NAME = re.compile(r'^Bench_(?P<cls>.+)_N(?P<n>\d+)_I(?P<i>\d+)_(?P<stamp>\d{8}_\d{6})\.csv$')
SWEEP1 = [(25, 1000), (50, 1000), (100, 1000), (200, 1000)]
SWEEP2 = [(100, 100), (100, 1000), (100, 5000)]
CUT_PER_END = 0.05  # ร้อยละ 5 ที่แต่ละปลาย = TRIMMEAN(ช่วง, 0.1)
DEFAULT_FOLDER = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'Saved', 'Profiling', 'CSV')


def frame_values(path, column='GameThreadTime'):
    with open(path, newline='', encoding='utf-8', errors='replace') as f:
        rows = csv.reader(f)
        header = next(rows)
        idx = header.index(column)
        values = []
        for row in rows:
            if not row or row[0] == 'EVENTS' or row[0].startswith('['):
                break
            try:
                values.append(float(row[idx]))
            except (ValueError, IndexError):
                continue
    return values


def trimmed_mean(values, cut=CUT_PER_END):
    v = sorted(values)
    k = int(math.floor(cut * len(v)))
    kept = v[k:len(v) - k] if k else v
    return sum(kept) / len(kept), len(v), k


def fmt(mean, sd, arm):
    digits = 1 if arm == 'A' and mean >= 100 else 2
    return f'{mean:.{digits}f} $\\pm$ {sd:.{digits}f}'


def main(folder, exclude):
    cells = {}
    for path in sorted(glob.glob(os.path.join(folder, 'Bench_*.csv'))):
        base = os.path.basename(path)
        if base in exclude:
            print('skip (excluded):', base)
            continue
        m = NAME.match(base)
        if not m or m['cls'] not in ARM:
            print('skip (name):', base)
            continue
        vals = frame_values(path)
        if not vals:
            print('skip (no GameThreadTime frames):', base)
            continue
        rep, n_frames, k = trimmed_mean(vals)
        key = (ARM[m['cls']], int(m['n']), int(m['i']))
        cells.setdefault(key, []).append(rep)
        print(f"{key[0]} N={key[1]:<4} I={key[2]:<5} {m['stamp']}  frames={n_frames:<6} trimmed {k}/end  rep={rep:.3f} ms")

    print('\nper cell (mean +- SD over runs):')
    summary = {}
    for key in sorted(cells):
        reps = cells[key]
        mean = statistics.mean(reps)
        sd = statistics.stdev(reps) if len(reps) > 1 else float('nan')
        summary[key] = (mean, sd)
        warn = '' if len(reps) == 3 else f'   <-- {len(reps)} run(s), protocol needs 3'
        print(f'{key[0]} N={key[1]:<4} I={key[2]:<5} n={len(reps)}  {mean:.3f} +- {sd:.3f} ms{warn}')

    print('\nderived (narration):')
    for n, i in SWEEP1 + [(100, 100), (100, 5000)]:
        a, b, c = (summary.get((x, n, i), (float('nan'),))[0] for x in 'ABC')
        print(f'N={n:<4} I={i:<5} A/B={a / b:7.1f}x  C-B={c - b:6.3f} ms  (C-B)/N={1000 * (c - b) / n:5.2f} us/actor')
    if all((x, n, 1000) in summary for x in 'AB' for n in (25, 200)):
        sa = (summary[('A', 200, 1000)][0] - summary[('A', 25, 1000)][0]) / 175
        print(f'Arm A slope in N at I=1000: {sa:.3f} ms per actor per frame')
    if all((x, 100, i) in summary for x in 'AB' for i in (100, 5000)):
        si = (summary[('A', 100, 5000)][0] - summary[('A', 100, 100)][0]) / 4900 / 100
        sb = (summary[('B', 100, 5000)][0] - summary[('B', 100, 100)][0]) / 4900 / 100
        print(f'Arm A slope in I at N=100: {1000 * si:.3f} us per iteration per actor')
        print(f'Arm B slope in I at N=100: {1e6 * sb:.2f} ns per iteration per actor')

    def row(first, n, i):
        out = []
        for arm in 'ABC':
            v = summary.get((arm, n, i))
            out.append(fmt(v[0], v[1], arm) if v else '-')
        return f'        {first} & ' + ' & '.join(out) + ' \\\\'

    print('\n% tab:ch14_instance_sweep (I = 1,000)')
    for n, i in SWEEP1:
        print(row(str(n), n, i))
    print('\n% tab:ch14_iteration_sweep (N = 100)')
    for n, i in SWEEP2:
        print(row({100: '100', 1000: '1{,}000', 5000: '5{,}000'}[i], n, i))


if __name__ == '__main__':
    if hasattr(sys.stdout, 'reconfigure'):
        sys.stdout.reconfigure(encoding='utf-8')
    args = sys.argv[1:]
    exclude = set()
    if '--exclude' in args:
        k = args.index('--exclude')
        exclude = set(args[k + 1:])
        args = args[:k]
    main(args[0] if args else DEFAULT_FOLDER, exclude)
