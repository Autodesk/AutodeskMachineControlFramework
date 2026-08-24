
</br>
</br>

![](./images/autodesklogolarge.png) 

</br>
</br>
</br>

# Introduction to the Autodesk Machine Control Framework


## 1. Background Information & Justification

The standard control software for contemporary Additive Manufacturing Machines significantly deviates in a few areas from conventional machine tools, like Milling and Turning machines. While in the latter the operating model is centered around the real-time PLC and complex motion control, in the Additive Manufacturing space (at least for Laser Powder Bed Fusion, Stereolithography and Inkjet technologies) the motion aspect is less critical on the one hand, but on the other hand it is essential to integrate optics and other control systems that are structurally equal, while operating at a much higher frequency (MHz clock rate) in comparison of common PLCs (kHz clock rate).

In addition to those requirements, the amount of data that needs to be processed is often several magnitudes above what is usually needed in traditional Manufacturing. Build files for large prints can easily reach up to 10 or 20 Gigabytes, easily exceeding the capacity of contemporary real-time control systems.

As a result, the mostly applied software architecture usually consists of a standard PC running the different hardware components in parallel, trying to feed them with the data needed and orchestrate their behavior. There might be a direct wire link between the subsystems for clock synchronization, but even this is not state of the industry.

![](./images/plcvspccentric.png) 

While the traditional space has seen some developments into more connected workflows (using for example UPCUA) in the recent past, but it is still very clear that the focus and central element of a machining process is still the G-Code-like program running in the PLC directly.


Given this speciality, every AM vendor is usually investing heavily in building its own PC control software stack completely from scratch - for a relatively low unit count. This leads to 
- a very fragmented market of proprietary solutions 
- many resource constraints of the necessarily small development teams, resulting in often low software quality
- many overlapping developments in different companies of the same functionality
- while building out their technological core differentiation only receives a tiny fraction of the overall effort.

As users of Additive Manufacturing Machine Tools in general operate a multitude of different machine formats and vendors, this fragmentation creates huge pains in the upstream and downstream process chain (from data preparation to quality control and postprocessing), being a real blocking issue for many applications to become main-stream.

The Autodesk Machine Control Framework is an open software collection that tries to address all of those issues in a structured manner. Similar how Android (TM) has been a backbone and level-setter for the mobile phone industry, it glues together the Upstream CAD/CAM data preparation pipeline with off the shelf hardware components that are sold in the market, trying to become a state of the art reference implementation of how a modern (additive) machine tool software stack should look like from the software side.

Taking away tedious repetitive reimplementations of non-value adding features, this has turned out to be an incredible productivity gain for software teams. Independent if they are trying to build up test systems, commercial grade machines or even retrofitting older machines with new control systems - the case available studies about the Framework's customizability show that it is easily possible for a small team of individuals to develop a completely novel state of the art machine tool within a few months - in industrial product quality.

While most of the work focuses on Laser-based technologies like Metal Laser Powder Bed Fusion and Stereolithography, the basic addressed challenges apply to a much wider range of technologies (like Injket, DLP or Binder Jetting), and also in a field outside of Additive Manufacturing. We do believe that there is a place for the concepts in traditional Manufacturing adjacent AM processes like DED or even Robotics as well, but to stay focused this is out of scope at the foreseeable future.

## 2. Example use case

The whole project was created around a first model use case, based on a tight integration of a CAD/CAM solution (like Autodesk Fusion) and a Machine Control System with a closed feedback loop. Fusion is Autodesk's main stream solution for Manufacturing, covering a wide range of functionality. From Mechanical Design, Generative and Implicit Design to Finite Element Analysis with a variety of Physics - to Manufacturing. The Manufacture environment in Fusion supports several processes, for example 3, 4 and 5 axis milling, turning, sheet-metal cuttong and also of course Metal Additive Manufacturing, both in DED and Laser Powder Bed Fusion. The idea is to provide a mainstream audience access to the technology, with an accessible ease-of-use as well an unbeatable price tag. Looking at the specifics, we assume a closed loop process like the following.

![](./images/exampleusecase.png) 

The desired model workflow steps are

- Prepare parametric models in CAD for Printing, which means orientation, support generation

- Simulate the process with a predictive FEA simulation

- Create and preview the laser toolpath depending on assigned settings at the CAD geometry.

- Automatically send the toolpath to the Machine Control, which handles data management,  hardware control, subsystem syncronization, error handling, etc..

- The hardware consists of off the shelf components from SCANLAB and B&R Automation, which then are used to melt the part.

- The process is monitored with cameras and off-the-shelf in-situ process monitoring solutions, for example a pyrometer from Sensortherm GmbH.

- The data is captured, fed back into the Machine Control Software, which aggregates several sensors from subsystems, and forwards an aggregate view back into the CAD system, where for example it can be overlayed with the earlier process simulation.

- In case there are deviations, or unwanted side effects, there is the possibility to run an outer process loop, which included adapting the model geometry, orientation, toolpathing or support strategy.

- Another option is to use the Control Framework's openness to directly create an in-process control of laser parameters dependent on a non-realtime measurement.

- The third control possibility is to directly use SCANLAB's off the shelf closed-loop control mechanisms to directly influence the melt-pool temperature to a specific regional target that was given in the CAD system or toolpathing kernel.

This workflow depicts pretty much the standard frontier technology state of Academia and the industry. It is important to highlight that the named software and hardware references are examples only, and the flexibility and openness of the framework shall encourage the mixing and matching of any sort of device in any price range. There is no vendor lock in of any kind. Having said that, the implementer's life will of course be much easier if he or she uses already supported components.

As a first principle, the target of this project is not to proof that it is possible at all to create such an implementation per se. The real goal was much more extensive: Creating a framework that allows a small team of engineers with sufficient knowledge of Mechanical Engineering and the AM Process to create such a system within a few months from scratch - without having to have decades of experience in Software Engineering on the one hand - but nevertheless in industrial grade quality on the other.

We strongly believe that this has the potential to free resources and speed up iterative developments. Maybe enough to really increase the velocity of technology developments. And all of this while creating a direct path to production ready deployment of results, an issue that is very endemic for Academic and Corporate Research organisations alike.

## 3. Acknowledgements

The work on this project was mainly pushed by Autodesk Inc. in its quest to make Additive Manufacturing an often applied mass market main-stream technology. But it would not have been possible without many participants and contributors, which are too numerous to name conclusively. As main industrial collaboration partners, we would like to highlight

![](./images/scanlablogosmall2.png) 

- SCANLAB GmbH, the world's leading manufacturer of Laser Scan Systems and other Hard- and Software Components that move laser spots around.

![](./images/brautomationlogo2.png) 

- B&R Automation (A member of the ABB Group), one of Europe's leading provider of PLC systems in general and for the Additive Manufacturing Market.

![](./images/nikonslmlogo.png) 

- Nokia SLM Solutions GmbH, probably the world's leading provider of large scale multi-laser systems. We are tremendously grateful for the exploratively openness of them to try out the framework for some of their farer out R&D technology projects.

The amount of advice and credibility that this collaboration has received from all of them is invaluable.

On the academic side, we are very grateful to our Institutional partners, which guard decades of application experience and were great first-movers to build out the software stack with real-world

- Technical University of Munich, Institute for Machine Tools and Industrial Management (iwb)

- Technical University of Munich, Professorship of Laser Based Additive Manufacturing

- University of Aalen, Laser Application Center (LAZ)

- University of Nottingham, Advanced Manufacturing Center

- ETH Zürich, Advanced Manufacturing Laboratory

- Technical University of Denmark

And finally we would like to thank the uncountable amount of high profile industry leaders on all continents who have let us waste their time to listen to our conceptual presentations. And then in return even gave us free of charge consulting with real-life feedback and requirements gathering that in aggregation nobody would have been able to come up with in isolation.

This project has probably been the most extensive and wholistic requirements engineering exercise that has been conducted so far in the Additive Manufacturing Industry.


## 4. Open Source methodology

In a commercial context, the machine control software often is seen a sensitive topic by a Machine Tool Maker. It directly touches the crown jewels of its intellectual property, and any proprietary outside dependencies are regularly subject to strict scrutiny. We strongly believe that any viable business proposal in this area must include a complete code ownership by the OEM.

There might be other commercial models supporting those constraints - like detailed maintenance contracts with source code escrows. However, as real world experience shows, the legal and monetary arrangements in those cases are extraordinary complex. The negotiation and execution of such contracts often proves to be very difficult, especially if they have to be uphold over long time periods.

The associated risks are hard to estimate. Small supplier companies at any time can go bankrupt in an upcoming crisis, larger corporations are prone to regular changes in strategy and even divestments of whole business lines. In the rare best case the extensive paperwork is well-written and does not contain inherent inconsistencies. But even then the Machine Tool Vendor might be legally protected and entitled to a continuous operation in theory - while in practice facing the reality of a sudden loss of technical access to his business critical software stack.

In our opinion, the cleanest, easiest, and most attractive option is to maintain a central Framework code under a permissive open source license. While this does not 100% avoid any maintenance risk, it at least opens a realistic path to independence should the supplier relationship become impacted in any form.

While there are many open source licenses that would fit, the selected one ideally should be as short and clear as possible. Self-perpetuating licenses like GPL or LGPL add complexity and are not in line with creating applications in commercial environment.


## 5. License

Following the above philosophy, if it is not explicitely mentioned otherwise (like for third party dependencies), all documents and code of this project fall under the BSD 3 clause license, as stated below. This includes commercial and non-commercial use without the need for the user to disclose proprietary modifications to the public.

____

Copyright (C) 2024 Autodesk Inc. 

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
*  Redistributions of source code must retain the above copyright
	  notice, this list of conditions and the following disclaimer.
*  Redistributions in binary form must reproduce the above copyright
	  notice, this list of conditions and the following disclaimer in the
	  documentation and/or other materials provided with the distribution.
* Neither the name of the Autodesk Inc. nor the
	  names of its contributors may be used to endorse or promote products
	  derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL AUTODESK INC. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

## 6. Contributions

Contributions are welcome and we are looking for people that can improve the core system as well supply new drivers or hardware interfaces.

You can also contribute by reporting bugs in the Issue tracker on github, helping review pull requests, participate in discussions about issues and more.

### Filing issues
1. When filing an issue to report errors or problems, make sure to answer these five questions:
	1. Which version of the Autodesk Machine Controller Framework are you using?
	2. Which operating system, programming language(s) and development tools (compiler/interpreter) are you using?
	3. What did you do?
	4. What did you expect to see?
	5. What did you see instead?
2. When contributing to this repository, please first discuss the change you wish to make via issue with the [maintainers](#maintainers) of this repository. This way, we can ensure that there is no overlap between contributions or internal development work.

3. Adhere to the code of conduct..

4. Review, sign and send in either the "Autodesk Machine Control Framework Form Corp Contrib Agmt for Open Source.pdf" if you want to contribute as a corporation or the "Autodesk Machine Control Framework Form Ind Contrib Agmt for Open Source.pdf" (both files are in the main folder of the official github repository) if you want to contribute as an individual.

### Submitting a pull request
When ready to contribute, fork this repository and submit a pull request that references the issue it resolves. Be sure to include a clear and detailed description of the changes you've made so that we can verify them and eventually merge.

The Autodesk Machine Controller Framework follows the [git-flow](https://www.atlassian.com/git/tutorials/comparing-workflows/gitflow-workflow) branching model. New developments are integrated into the [develop](../../tree/develop)-branch. The maintainers will create releases from the develop-branch when appropriate.

If you've established yourself as an impactful contributor to the project, and are willing take on the extra work, we'd love to have your help maintaining it! Email the current maintainers for details.


# I The AMCF Core System

## 1. Engineering Principles and Design choices

The following chapter shall outline the philosophical principles that were followed during the development, as well as the design choices that follow as consequence.


### 1.1. Separation of Framework and Machine specialization

In our decade-long experience in operating Additive Manufacturing machines, we have seen many different approaches to implement a proper machine control software stack. Most software solutions in the field are custom made for a certain machine series, with a large variation in certain aspects like user experience, but at the same time being very similar on what they do at the core. Depending on the market, history and development resources, a vendor would make or have to make different choices, and subject itself to different constraints. Looking wholistically, the overlap between different software stacks is striking none-the-less, and the question arises, if one could develop a common architecture for a majority subset of available systems.

The key to create such a reusable system is a proper level of abstraction. It is essential to capture most of the possible functionality of an industrial Additive Manufacturing system, and implement it into the core. But to adapt to different printing technologies, one also needs to create an extensive plugin system that allows to specialize the framework to most specific use cases.

We believe the core should at least cover the following areas, which are broadly applicable for most Additive Manufacturing Processes:

- A Deterministic execution logic on the system, containing all state defining variables.

- Proper Version control of all system components

- User management, support for user roles and authentication

- An established logging system, with hierarchical granularity

- A reusable mechanism for quick development of User Interfaces and HMI

- A concept how to include proprietary hardware and SDKs

- A backend relational database for storing meta data

- Data schema versioning and migration schemes for updating

- Proper binary data management (Build files, image data etc..)*

- Build data parsing and extraction

- Multi-threading and subsystem synchronisation


In addition to those specifics, more in general we believe the framework should also provide functionality that is usually shpped via third party libraries that are often painstakenly difficult to include in a large project. This includes

- XML, JSON and ZIP parsers/writers

- Generic TCP/IP Networking code

- Generic OPCUA connection functionality

- Other connectivity support, for example UART and ModbusTCP

- Statistics and Time Series

- Image compression, display and manipulation

- Slicing and rasterization algorithms

- Video capture, display and manipulation

- Date and time functionality, timing measurements

- Hash- and Cryptographic functions

- HTTP Clients and REST Servers

- LDAP and other identity protocols, Corporate single sign on

- OpenGL, 3D Graphics and Shader programming                   

- Cloud connectivity and remote control


None of the two dozen points above have any direct connection to a machine maker's process knowledge, and are usually just experienced as cost factor by the software team in charge. And they not only make up a small fraction of the overall effort - but span the majority of the resource investment. The reusuability of this costly, non-differentiating 

### 1.2. Multi-platform, Multi-user, Multi-language Design

While developing a framework with a platform concept in mind, dealing with as many use cases as possible must be one of the priorities. This means to support the big 3 M's:

**Multi-platform:** The code should compile and run on most hardware, operating systems and processor architectures. In our case we would make sure the codebase can compile on:

1. Windows x86, 32bit ("win32")

2. Windows x64, 64bit ("win64")

3. Linux x86, 32bit ("linux32")

4. Linux x64, 64bit ("linux64")

5. Linux ARM, 32bit ("arm32")

6. Linux ARM, 64bit ("arm64")

7. Mac OS X Intel ("mac64")

8. Mac OS X ARM ("macarm")


As those are many configurations, in order to balance complexity with the need of the majority of applications, we will publish precompiled binary releases for the top three platforms in the field (win64, linux64, arm32).

**Multi-user:** User authentication is a complex topic, which really should not be left to the specialization code. The framework should come with a proper login system and user database, which allows to record who did what and when in the operating and administration modes of the machine. The user experience should be tailored to a certain assigned user role, and there should be a permission mechanism to ensure that nobody can perform actions exceeding one's role priviledges.


**Multi-language**: In order to be a commercially viable software solution, the framework should also come with its internal internationalization scheme that allows it to support multiple client-facing languages in a dynamic way - which means the language setting should be on a per-user basis and be changeable either instantaneously or between logins.

### 1.3. Modular Architecture and the need for a Client-Server system

There are many interpretations of "modular architecture", but in AMCF the meaning is practical: each part of the system should be replaceable or extensible without rewriting the entire stack. A machine builder should be able to swap a driver, add a new build strategy, or customize the operator workflow while keeping safety logic, scheduling, and API contracts intact. Modularity only exists if separation of concerns is enforced by clear boundaries, stable interfaces, and explicit data ownership. This reduces integration risk, accelerates vendor onboarding, and prevents long-term drift between UI and machine behavior.

At the highest level, AMCF is organized into three layers: a deterministic core server that owns state, scheduling, and safety-relevant decisions; a client layer that renders the UI and communicates over a stable API; and a plugin/driver layer that integrates hardware and process logic. Each layer is independently replaceable as long as its interface contract is respected. The server is the single source of truth, the client is a consumer and operator interface, and the plugin layer is the only place that should know about proprietary device details.

This structure naturally leads to a client-server architecture. In industrial control systems, the UI is only one of several consumers of machine state and services. Other consumers include automated test harnesses, remote service tools, analytics pipelines, and manufacturing execution systems (MES). A client-server design formalizes that all machine functionality is exposed as a service usable by any authorized client. This is critical for integration with factory ecosystems, for multi-client access, and for consistent behavior across different deployment contexts.

The server is designed to run headless with identical behavior whether a UI is attached or not. It can run on an industrial PC, a rack-mounted server, or a virtualized environment for simulation and testing. Clients can be local (HMI), remote (service), or distributed. Because the server owns state and arbitrates actions, multiple clients can connect simultaneously without introducing inconsistent behavior. The server validates all actions under the same safety and scheduling constraints, ensuring that client diversity does not compromise correctness.

The client-server split also prevents tight coupling between UI logic and machine logic. In tightly coupled systems, UI features become de facto control logic and are hard to test or reuse. AMCF enforces that control actions are implemented in the server and exposed via API endpoints. The UI does not "own" functionality; it only presents it. This keeps control logic testable with automated clients and makes UI redesigns low risk with respect to core machine behavior.

Determinism and safety are major reasons to keep the server authoritative. Machine control requires predictable timing, state transitions, and interlocks. The server enforces these constraints without being affected by UI latency or network variability. By isolating timing-sensitive logic in the server, AMCF ensures safety-critical transitions execute under controlled conditions.

The plugin and driver layer is the mechanism that localizes hardware differences. Machines vary in sensors, actuators, motion controllers, scanners, cameras, and safety devices. AMCF isolates hardware integration into drivers and plugin interfaces so machine builders can tailor their hardware stack without modifying the core. Vendor-specific SDKs or protocols belong here, not in the server or client. This keeps the server stable and makes new hardware onboarding a localized change rather than a system-wide refactor.

Because drivers and plugins are modular, they can be tested in isolation. Simulation drivers can provide deterministic responses for unit and integration tests, and process logic can be exercised without real hardware. The same API surface used in production is used in tests, which reduces the need for custom harnesses and makes test results representative. A simulated server can also support UI development and regression testing without constant access to a physical machine.

From a deployment and operations perspective, the client-server model separates update cycles. UI features often evolve faster than core machine logic, so a workflow or dashboard can ship without touching the server. Conversely, a server security or safety patch can be deployed without rebuilding the UI, reducing downtime and validation effort.

Scalability, security, and resilience also improve. As machines become more connected, a server with a single authoritative state can serve multiple clients with controlled permissions. Authentication and authorization are enforced in one place, and client commands are validated as untrusted input. If a UI disconnects or a network link fails, the server continues operating safely and clients can recover state through the API.

In practice, the architecture supports common deployment patterns without special cases:

- Local HMI with server on the same machine for low-latency control.
- Remote UI clients over a network for monitoring and service.
- Headless server operation with automated clients or scripts for testing and integration.

All of these use the same API contract, which makes them predictable and easier to maintain.

Finally, modularity is essential for long-term evolution. Machines often live for decades, while UI technologies and hardware components evolve quickly. A stable server with versioned interfaces allows new clients, plugins, and drivers to evolve independently. This is a strategy for managing change across the lifetime of a machine, not just a technical preference.

In summary, AMCF’s modular architecture provides strong boundaries between control logic, hardware integration, and user interaction. The client-server split enables determinism, safety, multi-client support, independent updates, robust testing, and long-term maintainability. It allows AMCF to function as a platform rather than a single-purpose application, supporting diverse machines and workflows while keeping the core stable and reliable.

### 1.4. Plugin mechanism and choice of programming language

Every machine is a specialized system. The framework therefore exposes extension points for:

- Drivers (hardware connectivity)
- Process logic (build strategies, motion sequences)
- UI modules (custom operator workflows)

AMCF standardizes these extension points with a strict interface contract rather than ad hoc C++ headers. The contract is defined in XML and consumed by the Autodesk Component Toolkit (ACT), which generates the bindings, ABI headers, and wrapper code used across the framework. This is a deliberate design choice: interface definitions are treated as declarative specifications, while generated artifacts are treated as build outputs. Developers change the XML, regenerate, and then implement or consume the resulting interfaces. This avoids hand-crafted glue code, enforces consistency, and makes interface evolution explicit and auditable.

ACT XML files live alongside the framework and drivers. Examples include `ACT/LibMCEnv.xml` for core environment interfaces, `SDK/ACT/LibAMCF.xml` for the public SDK, and `Drivers/*/ACT/LibMCDriver_*.xml` for hardware drivers. These XML files describe the library namespace, versioning, error codes, data types, and the classes and methods exposed by the component. XML is used because it is concise, diff-friendly, and easy to validate. It also scales well across different interface scopes: a core server API, a single driver, or a UI plugin can all be expressed with the same schema.

The XML definition is the single source of truth for interface shape. It captures:

- Library identity and version metadata (library name, namespace, base name).
- Error codes and error descriptions to keep API behavior consistent.
- Types, enums, and data structures shared across components.
- Class and method signatures, including parameters and return values.
- Import relationships between components, where one interface depends on another.

From that XML, ACT generates the bindings and ABI layers that allow components to interact safely. In this repository, the generated output includes C and C++ headers, dynamic loader wrappers, ABI definitions, and language-specific bindings for the SDK. You can see these outputs in folders such as `Framework/HeadersCore`, `Framework/InterfacesCore`, `SDK/Headers`, and in each driver’s `Headers` and `Interfaces` directories. Generated source and header files are clearly marked as ACT output; they should not be edited directly. The workflow is always: edit the XML, run ACT to regenerate, and then update implementation code as needed.

This approach also explains the choice of programming language. Plugins and drivers are implemented in C++ because machine control and hardware access require deterministic performance, tight memory control, and straightforward integration with existing SDKs. C++ also maps well onto the ABI layer that ACT generates. However, by using ACT-generated bindings, AMCF can expose those same interfaces to additional languages where appropriate, especially for higher-level integration and client tooling. The SDK includes bindings in multiple languages (for example C, C++, Python, and NodeJS), which makes it possible to integrate with external systems or build tooling without re-implementing the core logic. The interface surface remains the same because it is defined once in XML and re-generated for each target language.

ACT is also what makes versioning and backward compatibility tractable. Because interfaces are declarative, changes can be reviewed in a single file and then propagated to all bindings in a controlled manner. This reduces the risk of ABI mismatches between the server, plugins, and clients. It also means that changes to error codes, types, or method signatures are visible and can be enforced during code review, which supports long-term maintainability and supports multiple platform builds without custom glue.

For driver development, the XML approach is particularly valuable. A driver can import core components (for example `LibMCEnv`) to reuse shared types and services while defining only the driver-specific API. The generated headers provide a consistent interface to the server, while the implementation stays focused on vendor SDK integration and device-specific logic. This keeps driver code smaller and more predictable, and it avoids diverging conventions from one driver to the next. The same mechanism is used for UI plugins and process logic modules, which makes the overall plugin ecosystem easier to extend and document.

In short, AMCF’s plugin mechanism depends on ACT as the interface definition and code generation layer. XML definitions make contracts explicit, reduce ambiguity, and allow consistent bindings across languages. C++ remains the primary implementation language for performance and control, while ACT-generated bindings open the ecosystem to other languages for integration, tooling, and client development. This combination gives AMCF both the low-level control required for machine operation and the flexibility needed for long-term platform growth.

### 1.5. The Hourglass pattern and Stable ABIs

The hourglass model in AMCF is realized through ACT’s ABI strategy. At the top, you can have a rich, high-level class architecture: interfaces, inheritance, and structured types that make sense to an SDK user or driver author. At the bottom, you can have many different consumers and host environments that each want a simple, stable way to call into that architecture. The narrow middle is a plain C ABI that acts as the stable contract between them.

ACT takes the declarative interface definition and generates a flat C DLL surface. Each class instance becomes a handle, and each method becomes a function that takes that handle plus serialized parameters. The C ABI uses standard error codes for all calls, which ensures a consistent failure model regardless of language or runtime. This layer is intentionally minimal: no C++ name mangling, no exceptions, no STL types, and no compiler-specific ABI details. The result is a binary interface that remains stable across compiler versions and across languages.

On the consumer side, ACT-generated bindings reconstruct the richer class model. The C ABI functions are wrapped into language-native classes or interfaces, exposing idiomatic method calls, typed parameters, and resource lifetimes. From the point of view of a client, it feels like using a normal SDK. Under the hood, each call is routed through the C ABI, with error codes translated into language-native exceptions or return types. This "expand again" step is what preserves developer ergonomics while keeping the ABI compatible and predictable.

The benefit of this pattern is that the complex class hierarchy is not the ABI; the ABI is the thin C layer. That means new languages can be supported by generating new wrappers without changing the core, and drivers or plugins compiled with older toolchains can keep working against newer servers as long as the C layer remains compatible. It also means that runtime loading becomes simple and uniform: every plugin is just a DLL with a defined set of exported C functions, and every consumer speaks the same function-level contract.

This model also makes it straightforward to integrate proprietary third-party code. A plugin or driver can link against vendor SDKs, hardware libraries, or closed-source toolkits internally while exposing only the stable C ABI to the outside. The proprietary dependency stays encapsulated behind the handle-based interface, so the rest of the system does not have to understand or adapt to vendor-specific binaries, compilers, or error models. As long as the plugin respects the ABI contract, the core remains unaffected and stability is preserved.

The hourglass approach helps manage change at different speeds. Vendor SDKs may update frequently, operating systems may add new requirements, and clients may demand new UX capabilities. Those changes can be absorbed at the edges without forcing a ripple through the system, because the middle remains narrow and stable. ACT’s generated C ABI becomes the governance point for compatibility, allowing the framework to enforce versioning, detect mismatches early, and keep older components operational during a transition.

In practice, this means API boundaries are strict. Input and output types are explicit, error codes are formalized, and versioning is part of the design, not an afterthought. The narrow middle becomes a safe place to stand: a known set of behaviors that clients and drivers can trust. The wide edges remain free to innovate, but they do so by speaking the same stable language. Over time, this approach protects the system from fragmentation, keeps long-lived deployments supportable, and allows AMCF to grow in capability without sacrificing reliability.

### 1.6. API First Design guidelines and best practices

API-first means the server API is the primary product surface, and everything else
(UI, scripts, automation, and integrations) is a client of that API. If a feature
is important for operators or external systems, it must be reachable through a
documented endpoint. This keeps the behavior consistent across clients, reduces
hidden state, and makes integration predictable. It also forces the team to think
about stability and supportability before a feature ships.

API-first has a few core implications for the framework. The server owns the
domain model and state transitions; the UI only visualizes and orchestrates.
Business rules live in the server API layer so that a button click and a remote
integration call have the same effect. Features must be implemented as API
capabilities first, then consumed by UI or plugins. This prevents UI-only logic,
ensures testability, and avoids subtle discrepancies between local and remote
operation.

To keep the API reliable and evolvable, design endpoints as explicit contracts.
Every request should return structured data and explicit error codes, never rely
on ad-hoc text or ambiguous HTTP status mapping. Favor clear, typed payloads over
deeply nested or under-specified blobs. When a request changes server state,
make the operation explicit and document the required preconditions (e.g.,
machine state, permissions, or resource locks). Where possible, design actions
to be idempotent so clients can retry safely. Use pagination for lists, stable
identifiers for resources, and consistent naming across endpoints.

Versioning is non-negotiable. Any breaking change must be introduced as a new
API version, with existing versions kept operational for a defined period. Add
new fields in a backward-compatible way and avoid changing semantics of existing
fields. Deprecate with a clear migration path, and document the planned removal
window. This approach protects older UI packages and external integrations while
allowing forward progress. In practice, API versioning should be reflected in
endpoint paths and in the generated client bindings where applicable.

Security and permissions must be enforced at the API boundary. Do not depend on
UI behavior for access control. Every endpoint should validate permissions based
on roles and session context, and return clear errors when access is denied. If
an action has safety implications, the API should require explicit confirmation
or a stronger permission scope, and that requirement should be documented.

Observability is part of the contract. API calls should generate structured
logs, meaningful error codes, and correlate with build/job identifiers when
relevant. This makes it possible to trace behavior in the field and tie a
failure to a specific build, configuration, or machine state. For long-running
operations, provide status or progress endpoints and consistent polling or event
mechanisms. Avoid hidden background tasks that cannot be queried or cancelled.

Finally, design the API so it is pleasant to consume. Documentation should
describe the purpose of each endpoint, the request and response models, and
example payloads. Keep names consistent, provide clear field descriptions, and
call out invariants. A simple rule is to treat the API docs as a first-class
deliverable that changes in lockstep with the code. Tests should validate both
success and failure cases, and new features should add API-level tests alongside
implementation changes. When in doubt, ask: could a third-party integration
reliably use this endpoint without reading the server code? If the answer is
no, the API is not yet first-class.
### 1.7. Serialization principles, XML, JSON and binary storage

### 1.8. Resource handling and file access

### 1.9. Database backend, Schemas and Data Migration

## 2. Engineering Lifecycle Considerations

### 2.1. Code degradation and successful development over many years

### 2.2. Using of git hashes in the core code

The framework embeds git hashes into build artifacts to track provenance. This allows the machine operator and developer to identify the exact source revision of a running system, which is critical for:

- Debugging field issues
- Reproducing test results
- Verifying compatibility of plugins and packages

Build scripts store hashes in `build_<platform>/githash.txt` and package metadata, and the server exposes this information in its output folder and logs.

### 2.3. Stable APIs, Backwards compatibility and Data migration

### 2.4. Separation of Data and Logic and forward compatibilty.

### 2.5. Package management, CI/CD and proper build pipelines.

### 2.6. Testing Framework

Testing is integrated into the CMake build and includes both unit tests and system tests. The unit test framework in `Implementation/UnitTest` is built into the `amc_unittest` binary, while driver tests live under `Tests/<DriverName>Test`. Tests are expected to run against deterministic inputs and use simulation drivers where possible.

## 3. General Backend Architecture

### 3.1. Client Server Implementation

### 3.2. Finite State Machines

### 3.3. Signals and Concurrency

### 3.4. Driver Concept and Statefulness

### 3.5. Build data management

### 3.6. 3MF Toolpath and Metadata

### 3.7. Logging and Journaling

### 3.8. Determinism and Replay

### 3.9. Implementation constraints

### 3.10. Resource sharing and driver locks

### 3.11. Consuming proprietary SDKs

## 4. Machine specialization

### 4.1. The Configuration XML

### 4.2. Language Definitions

### 4.3. Permissions and Role models

### 4.4. Alert System

### 4.5. State machines, Variables and Signal Definitions

### 4.6. Plugin Mechanism

### 4.7. The API Environment

### 4.8. Essential StateEnvironment Methods

### 4.9. A minimal machine flow example

## 5. Advanced API Functionality



# II The AMCF User Interface System

## 1. Generic Concept

### 1.1. Pages, Dialogs and Modules

### 1.2. Overview of Widget system

### 1.3. Custom pages

### 1.4. User Interface Plugin Mechanism

### 1.5. The API Environment

### 1.6. Events and Essential UIEnvironment Methods

## 2. Module Documentation

### 2.1. Content Module

### 2.1.1 Paragraph Widget

### 2.1.2 Image Widget

### 2.1.3 Form Widget

### 2.1.4 ButtonGroup Widget

### 2.1.5 Chart Widget

### 2.1.6 Upload Widget

### 2.1.7 ParameterList Widget

### 2.1.8 BuildList Widget

### 2.1.9 AlertList Widget

### 2.2. LayerView Module

### 3.3. Logs Module

### 3.3. Graphic Module

### 3.4. GLScene Module

### 3.5. Grid Module 

### 3.6. Tabs Module 


# III Standard Driver Documentation

## 1. PLC and Motion Drivers

### 1.1. B&R Automation TCP/IP Driver

### 1.2. Beckhoff ADS Driver

### 1.3. Hilscher cifX Driver 

### 1.4 Siemens S7PLC Driver

### 1.5 Technosoft TML Driver

### 1.6. Aerotech A3200 Driver

### 1.7. Standa XIMC Driver

## 2. Scanner Drivers

### 2.1. SCANLAB RTC6 Driver

### 2.2. Raylase Driver

###  2.3. HA Laser Systems Driver

## 3. Open Communication Drivers

### 3.1. OPCUA Driver

### 3.2 TCP/IP Driver

### 3.2 UART Driver

### 3.3. GRPC Driver

### 3.4. Marlin Driver

## 4. Auxiliary Device Drivers

### 4.1. SCANLAB Open Interface Extension Driver

### 4.2. SCANLAB scanMotionControl Driver

### 4.3. FLIR Spinnaker Driver

### 4.4. Rasterizer Driver

### 4.5. Beckhoff BK9xxx Driver



# IV Development environment

## 1. C++ Compiler, CMake, Git

## 2. Setting up a Developer Environment on Windows

## 3. Setting up a Developer Environment on Linux x64

## 4. Setting up a Developer Environment on Linux ARM

## 5. Code Dependencies


# V Custom Driver Development

# VI Features

# VII Requirement Engineering and Future Roadmap

# VII Tutorials

# VIII AMCF REST API Documentation
