# Yo, I thought this was due like a month ago

After finishing (sort of) Project 2, I learned a ton of things. In particular, I learned a couple of coding principles that might make codeing a lot smoother and maybe even faster.

So I've decided to rewrite this whole project more or less from scratch, using the following principles:


<pre>
	1. Every file's functions and constant variables have a prefix. For example, functions in "linked_list.c" would have the format "ll_func_name(...)""

			(Test funcs, however, have a different format: "test_prefix_func_name()")

	2. Single Responsibility Principle: If I'm storing a group of closely-related data and manipulating it w/ functions, it belongs in its own "class" (c file).

	3. Outlining: I work best when I have a clear outline and a good grasp on what everything does. Therefore, before I actually code any c files, I'm going to write an outline in VERY basic UML.

	Here's an exmaple w/ sample.c:
			

			(Note that C stands for const)
			Vars:
					+ a: type
					+ b: type

					- c: type
					- d: type

					C e: const type 
					C f: const type
					C g: const type

			Funcs:
					init_data(): void
					destroy_data(): void
					...


	4. Global and static vars: All files that keep track of global and static vars should organize it within a struct. Here's an example w/ sample.c 


			sample.h:
					typedef struct {
							type a;
							type b;
					} Sample_Public_Data;

					extern Sample_Public_Data* sample_globals;

			sample.c

					typedef struct {
							type c;
							type d;
				} Sample_Private_Data;

				// Constants go here
				const type e = ...;
				const type f = ...;
				const type g = ...;
				...

				Sample_Public_Data* sample_globals;
				static Sample_Private_Data* sample_locals;


			// later...
			init_data() // Initializes public and private data
			destroy_data() // Resets and frees public and private data



	5. Test EVERYTHING


			Every func in UML should have a test written for it
			Also, figure out how each given test works


			WRITE THE TEST FIRST, then write code around it

</pre>


# Goals for this project


<pre>
	1. Refactor the code using these new principles
	2. End up with clean(er) code and basic UML docs for every class
</pre>






