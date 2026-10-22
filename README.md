# ชุดโค้ดประกอบ — สถาปัตยกรรมปัญญาประดิษฐ์สำหรับเกมด้วย C++ และ Blueprint ใน Unreal Engine

ชุดโค้ดประกอบ (Companion) นี้บรรจุโค้ด (Code) ฉบับสมบูรณ์ที่ทำงานได้จริงของทุกบทที่มีการลงมือปฏิบัติในงานเขียน ไม่มีส่วนที่ผู้อ่านต้องเดาหรือเติมเอง จัดทำขึ้นสำหรับผู้อ่านที่ต้องการลงมือทำตามไปพร้อมกับการอ่าน และสำหรับผู้อ่านที่ติดขัดระหว่างทางและต้องการสถานะที่ทำงานได้ถูกต้องแน่นอนไว้เปรียบเทียบกับงานของตนเอง

## ความต้องการของระบบ

- Windows 10 หรือ Windows 11
- Unreal Engine 5.8 (ตั้งแต่บทที่ 2 เป็นต้นไป)
- JetBrains Rider 2026.x หรือใหม่กว่า (ใช้ได้โดยไม่มีค่าใช้จ่ายสำหรับงานที่ไม่ใช่เชิงพาณิชย์)
- Microsoft Visual C++ Build Tools (Build Tools for Visual Studio) พร้อมเวิร์กโหลด (Workload) "Desktop development with C++" และ ".NET desktop build tools" (ขั้นตอนอยู่ในภาคผนวก ก ของงานเขียน)
- .NET runtime รุ่นที่ UnrealBuildTool ของ UE 5.8 ต้องการ ตรวจสอบรุ่นที่ต้องการได้จากค่า `framework.version` ในไฟล์ (File) `Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.runtimeconfig.json` ของเอนจิ้น (Engine) แล้วเทียบกับผลของคำสั่ง `dotnet --list-runtimes` หากไม่มีรุ่นหลักที่ตรงกัน ให้ดูวิธีแก้ในหัวข้อ "ปัญหาที่พบบ่อย" ของ `docs/ch02.md`

การเปิดโปรเจกต์ (Project) Unreal ทำโดยเปิด File `HAPBook.uproject` ด้วย Rider โดยตรง ไฟล์ Solution (Solution File) ที่ Engine สร้างขึ้นเป็นทางเลือกสำรองเท่านั้น

Companion ทั้งหมดได้รับการทดสอบบนสภาพแวดล้อมข้างต้นเท่านั้น ซึ่งเป็นสภาพแวดล้อมเดียวกับที่ใช้ทดสอบเนื้อหาในงานเขียน

## จุดเริ่มต้น

ผู้อ่านเริ่มจากคู่มือประจำบทใน `docs/chNN.md` เสมอ คู่มือแต่ละบทระบุขั้นตอนการเปิดและการรัน (Run) จุดตรวจสอบผลลัพธ์ที่ควรได้ และจุดสะดุดที่พบบ่อยพร้อมแนวทางแก้ไข

| บท | Code | คู่มือ |
|---|---|---|
| 1 | `ch01-cpp/Ch01.sln` (Solution สำหรับ Rider ประกอบด้วย Console Project 8 Project) | `docs/ch01.md` |
| 2 เป็นต้นไป | `HAPBook/` (Project UE 5.8 เปิดด้วย `HAPBook.uproject`) | `docs/ch02.md`, `docs/ch03.md`, … |

## โครงสร้าง

```
ch01-cpp/        บทที่ 1: C++ มาตรฐาน เปิดด้วย Rider ผ่าน Ch01.sln (ยังไม่ใช้ Unreal Engine)
HAPBook/         บทที่ 2 เป็นต้นไป: โปรเจกต์ Unreal Engine 5.8 หนึ่งโปรเจกต์ที่เติบโตทีละบท
  Content/Maps/L_ChNN   แผนที่ประจำบท: Actor สาธิตของบทนั้นวางไว้แล้ว เปิดแล้วกด Play ได้ทันที
  Content/Maps/L_Book   พื้นเปล่าสำหรับใช้เป็นแม่แบบ ไม่มี Actor สาธิต
  Content/ThirdPerson/  แผนที่และ Blueprint ของ Template Third Person (ใช้ตั้งแต่บทที่ 7)
  Source/HAPBook/       คลาส C++ ของงานเขียน (ที่ราก) เคียงข้าง AHAPBookCharacter/GameMode/PlayerController ของ Template
  */Variant_*/          ตัวอย่างแนวเกมของ Epic (Combat, Platforming, SideScrolling) ไม่ใช้ในงานเขียน
docs/chNN.md     คู่มือประจำบท
```

ทุกบทที่มี Code มีแผนที่ของตนเอง `L_ChNN` ซึ่งวาง Actor สาธิตของบทนั้นไว้แล้ว คู่มือประจำบทจึงเริ่มจากการเปิดแผนที่แล้วกด Play โดยไม่มีขั้นตอนวาง Actor แผนที่ของบทในส่วนปัญญาประดิษฐ์ (Artificial Intelligence: AI; บทที่ 7–13) เป็นสำเนาของแผนที่บทก่อนหน้าบวกสิ่งที่บทนั้นเพิ่ม ส่วน `L_Book` คงเป็นพื้นเปล่าเสมอ

`ch01-cpp/` เป็น Solution เอกเทศสำหรับบทที่ 1 เท่านั้นและไม่ถูกนำไปใช้ต่อ งานบน Unreal Engine เริ่มที่ `HAPBook/` ณ แท็ก (Tag) `ch02`

## หลักการ: หนึ่งโปรเจกต์ที่เติบโตทีละบท

`HAPBook/` คือ Project ที่สร้างจากแม่แบบ (Template) **Third Person (C++)** ของ Epic ใน Unreal Engine 5.8 ตามที่ได้จากหน้าต่างสร้าง Project โดยไม่ดัดแปลง (ไม่มี Starter Content) เพิ่มเพียงพื้นเปล่า `Content/Maps/L_Book` และแต่ละบทคือรากฐานของบทถัดไป Template ให้คลาส (Class) C++ สาม Class ที่รากของโมดูล (Module) ได้แก่ `AHAPBookCharacter`, `AHAPBookGameMode` และ `AHAPBookPlayerController` พร้อม Blueprint ลูกของแต่ละ Class ใน `Content/ThirdPerson/Blueprints/` (`BP_ThirdPersonCharacter`, `BP_ThirdPersonGameMode`, `BP_ThirdPersonPlayerController`) และแผนที่ `Content/ThirdPerson/Lvl_ThirdPerson` ซึ่งเป็นตัวอย่างของสถาปัตยกรรมแบบผสมผสาน (Hybrid Architecture Pattern: HAP) ในตัวเอง (คลาสฐาน (Base Class) เป็น C++ คลาสลูก (Derived Class) เป็น Blueprint) เนื้อหาส่วนนี้ใช้ตั้งแต่ส่วนที่ 3 (บทที่ 7) เป็นต้นไปเพื่อให้มีตัวละคร (Character) ที่ควบคุมได้ ส่วนบทที่ 2–6 ใช้พื้นเปล่า `L_Book` เท่านั้น โฟลเดอร์ (Folder) `Source/HAPBook/Variant_Combat/`, `Variant_Platforming/`, `Variant_SideScrolling/` และ `Content/Variant_*/` เป็นตัวอย่างแนวเกมสามแบบที่ Epic แถมมากับ Template ไม่ได้ใช้ในงานเขียน Class ของงานเขียนทุก Class อยู่ที่รากของ `Source/HAPBook/` เคียงข้าง Class ของ Template ระบบ AI ในงานเขียนถูกสร้างขึ้นทีละส่วนตั้งแต่บทที่ 7 ถึงบทที่ 13 บน Project เดียวกันนี้

สถานะของ Project เมื่อสิ้นสุดแต่ละบทคือ git Tag ชื่อ `chNN` ซึ่งรับได้สองรูปแบบ

- **zip File ของ Tag (วิธีหลัก ไม่ต้องใช้ git):** ดาวน์โหลด zip File ของ Tag `chNN` จากหน้า Releases (หรือ Tags) ของ repository — GitHub สร้าง File นี้ให้อัตโนมัติ ชื่อ File เป็น `<ชื่อ repository>-chNN.zip` แตก File แล้วเปิด Project ผู้อ่านจะอยู่ที่สถานะเดียวกับผู้อ่านที่เพิ่งจบบทนั้น
- **git tag ชื่อเดียวกัน (ทางเลือกสำหรับผู้ใช้ git):** `git checkout ch07` ให้ผลเช่นเดียวกับ zip ของบทที่ 7 และ `git diff ch07 ch08` แสดงทุกบรรทัดที่บทที่ 8 เพิ่มเข้ามา

บทที่เป็นแนวคิดล้วน (บทที่ 3, 15 และ 16) ไม่มี Code ใน Companion

## สถานะ

| บท | Code | tag | สถานะ |
|---|---|---|---|
| 1 | `ch01-cpp/Ch01.sln` | — | ครบถ้วน ผ่านการ Compile และตรวจสอบผลลัพธ์แล้ว |
| 2–14 | `HAPBook/` | `chNN` | อยู่ระหว่างจัดทำ |

## เกี่ยวกับโค้ดชุดนี้

Code ทุก File ตรงกับตัวอย่างชุดคำสั่งในงานเขียน และให้ผลลัพธ์ตรงกับที่งานเขียนแสดง หากพบความไม่ตรงกัน กรุณาแจ้งผ่าน Issues ของ repository นี้
