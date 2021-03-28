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





# Oh My God I'm Done

Now that I've actually finished, here are a few things I've learned along the way.

1. The Best Thing:

	The best thing about TDD isn't necessarily it's speed (although in the end I'm sure this will end up being faster than not doing TDD).

	It's safety. With TDD, I can basically guarantee that each function works.


Tests are just the best
		Need to explicitly write code around tests, not write tests around code
			E.g. if the tests call pfs_hostname(), then everything you need to 
			get the hostname() should be called in pfs_hostname()

			Struggled so much before b/c I tried to do some cool init() and destroy() stuff
			in display.c, but was never called in the tests
	
			So would either have to init() and destroy() a struct holding 1 value every time I called pfs_hostname(),
				or would just have to change the way I've been working on this

		Don't fear the heap, plan around it
			First time, my code was a lot harder to read and debug b/c I wouldn't use strdup()
			I didn't really know it existed, but I was against doing it anyway
			B/c I was afraid of leaks
			But things like strdup exist to make persisent data, and persistent data is safer than
			a lot of other string copying things

			Definitely going to use strdup a lot more often

		Convenience utility functions, yes yes yes
			free_string(), free_line_and_token(), the extract_token() funcs, and the search_file() funcs actually carried more than twice their weight
			also open_path(), which was from lab code
			Like I used them in 70% of the tests

			Having a general utility func for something I do often basically makes life a ton easier

			Should be constantly looking out for opportunities to make/use them - they'll pay off if I even use them 2x in 
			a project (but if I guess right, I'll be using them a LOT more than 2x)

			Also, I'm gonna be taking the following funcs with me wherever I go in this class:
			lineread()
			free_string()
			open_path()
			extract_token_before()
			extract_token_after()
			free_line_and_token()



		Abstraction can make life easier

			If I use abstraction in the following two ways, I can make code easier to understand (and thus,
				probably easier to debug):

			1) Abstraction through encapsulation:
				For each task I want to do (e.g. get the hostname), I should have a "driver function:"
					a function that calls a bunch of other helper funcs and holds some vars to 
					do the task


					The helper funcs themselves should be a black box if you're not looking at
					their implementation - should know generally what they do but not exactly
					how they do it
		
					That's helpful b/c now you DON'T HAVE TO keep track of how they do it
					If it works like you expect, you can dedicate brainpower to other things


				Then in helper funcs, can work out exactly how this sub-task should be carried out


				E.g. pfs_hostname():
					 char* hostname = search_for_hostname(proc_dir);
   					 int return_val = n_copy_if_present(hostname_buf, hostname, buf_sz);

   					 destroy_line_and_token(&hostname, NULL);
    					 return return_val;

				It's just 4 lines of code - 3 helper funcs, 1 return statement
				
				But (at least to me) it's clear what pfs_hostname() does:
					1. call search_for_hostname(...) and set a char * called hostname to the result
					2. call n_copy_if_present(...) - which likely the value to another place if the value exists - 
						and set an int called return_val to the result
					3. destroy the line and the token (but here, we only have the line)
					4. return return_val.

				This lets me focus more on how I want the func to work at a high level -
					I can think of what I want the computer to "do" in terms of actions instead of
					specific printfs or how much memory to allocate
					
			2) Abstraction through generalization:

				One of the definition of "clean code" from the book "Clean Code" is code that establishes
				abstractions early. For example, instead of writing a function that does something with
				just an array, rewrite it to work over ALL collections (Clean Code).

				For example: free_string(...) and destroy_line_and_token(...)

				Originally they were just a block of ocde in pfs_hostname, but then I realized I could
				write a function that would be helpful to basically every other function in the project

				This makes it easy to understand what's going on (destroy_line_and_token in one file
				works the same as in the next), but also lets me reuse code

		Code smaller
			If a class is >100 lines of code, even I don't want to read it
			If a func is >20 lines of code, I feel bad

			By coding smaller, code becomes easier to understand (and therefore, debug)

		Tests
			Honestly think I overdid it with the tests

			Was REALLY happy that I had so much test coverage

			Next time, though, probably going to move tests to their own files
			e.g. test_util.c or smth like that

			In addition, will only be testing:
				1) Public funcs
				2) Very general helper funcs (used by >1 public funcs)
					Will have to make general helper func public only during testing

			Helper funcs that only are called by one public func don't necessarily have to be tested
				At least for something smaller like this

			Instead, public func test should work ONLY if helper funcs work
			Also, can use a lot of logging


		How I think I should code

			Coding is a lot like writing. The first time you write it, it is inevitably hot garbage.
			
				The best thing to do is to just accept your first draft will be bad, churn out
				that hot stinky doo doo dumpster fire, and then rewrite it again and again until
				it's good.

			My friend Levin Weinstein gave me some advice: "TDD is my bread and butter:
				Having a test-main where you verify your code
				And only writing the most basic thing that you need to pass a test, then adding the next test
				Until you have a full feature...TDD is your breathing."
				"Since you test everything in TDD as you add it, if the next thing you build breaks, you know that all of the previous components work"

			That more or less convinced me to try TDD as soon as I remembered he said that, so I looked up what TDD was

			As far as I know it basically works like this:

				Given a behavior you want to code:
				1) Make a test for that behavior
					Tests to make sure func works properly
					Happens BEFORE you write the func, so that when you do write the func it's focused only on passing the test
					This prevents you from adding unnecessary features

				2) Refactor the test until it works
					It should run, even if the func hasn't been implemented yet (prob just use a placeholder like "return -1")
					It should fail, b/c the func hasn't been implemented yet 
						This is to check for false positives

				3) Actually write the code
					Yeah just do it
					From Wikipedia: "Write the simplest code that passes the neew test. Inelegant or hard code is acceptable,
					as long as it passes the test"

				4) Test the code with the test you wrote
	
				5) Refactor until the code passes the test
					
				6) Refactor until the code looks good
					This is where you make the code elegant and remove hard-coding


			So with that in mind, here's my new process:

				Given a behavior I want to code, I will:
			
				1) Make a placeholder func
					Basically:
						func(params):
							return (-1 or NULL);
				2) Make a test for that behavior
					Prob put test in test file

				3) Refactor the test until it works:
					Should run
					Should fail

				4) Write a shitty func
					Let it be big and clunky and slow, just let do the thing I'm trying to do


				5) Refactor func until it's a shitty func that passes the test
					This is where I'd move variables or call different funcs - basically debug the func
					NO making the code look good, that's for after we pass

				6) Refactor func until's a good func that passes the test
					Make it smaller, more elegant, and more efficient now

			Gonna try to do this w/ EVERY func until I memorize it

Lastly, there's something cool I found out with my project

Say there's a a file that reads info from a file and uses it to populate a struct.

Can actually create two structs:

1. Prefix_File_Read_Info: holds information from reading file
2. Actual_Struct: the actual struct that we want to populate

Can read file and populate prefix_file_read_info, then copy them over to Actual_Struct after

Although thinking about it this way, it might just be more efficient to format the data in a function, and add the data directly to Actual_Struct...


Over-all, I'm happy with how I did this project, and it's definitely helped me get better at coding. Maybe I'll try this again with Project 2. Or not - Project 2 made me sad :(


