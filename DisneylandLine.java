import path.to.exceptions.ListEmptyException;
import java.util.LinkedList;
import java.util.List;
import path.to.exceptions.InvalidPositionException;

//Put all classes in 1 file so its easier to read

/*
 * DisneylandLine
 *
 * A class to simulate a lineup of people waiting to enter
 * Disneyland. Some people come alone and go to the back of
 * the line, others bribe their way to a certain position.
 * Sometimes someone buys a bunch of tickets at once so
 * multiple people can be removed from the line and enter
 * Disneyland at the same time.
 */
public class DisneylandLine {
	List<Person> list;

	public DisneylandLine() {
		list = new LinkedList<Person>();
	}

	/*
	 * Purpose: add a person to line to enter Disneyland
	 * Parameter: Person p - the person to add
	 * Returns void - nothing
	 */
	public void enterLine(Person p) {
		if (p == null){
			return;
		}
		list.addBack(p);
	}

	/*
	 * Purpose: handle the first person in line
	            (allow them to purchase their ticket
				 and enter into Disneyland)
	 * Parameter: none
	 * Returns: Person - the person who gets their ticket
	 *                   and is no longer waiting in line
	 *                   or null if no one can be removed
	 *                   from the line
	 */
	public Person handleOne() throws ListEmptyException {
		if (peopleInLine()==0){
			return null;
		}
			try {
				return list.removeFront();
			} catch (ListEmptyException o){
				System.out.println("No one can be removed from the line.");
			}
	
		return list.removeFront();
	}

	/*
	 * Purpose: handle a whole group of people waiting in line
	 *          (allow a group to buy a number of tickets
	 *           and all enter Disneyland)
	 * Parameters: int num - the number of people
	 * Returns int - the number of people who were successfully
	 *               able to be removed from the line
	 */
	public int handleMultiple(int num) throws ListEmptyException{
		int num_people = 0;
		if(peopleInLine() == 0){
			return 0;
		}
    try {
        for (int i = 0; i <= peopleInLine() && i < num; i++) {
            list.removeFront();
            num_people++;
        }
    } catch (ListEmptyException e) {
        System.out.println("No one can be removed from the line.");
    }
    return num_people; //so it compiles
}

	/*
	 * Purpose: accept a bribe to put someone into a specific
	 *          position in the line to get into Disneyland
	 * Parameters: Person p - the person entering the line
	 *             int pos - the position they are trying to get to
	 * Returns boolean - true if person added to line, false otherwise
	 */
	public boolean premiumEntry(Person p, int pos) throws InvalidPositionException{
		if(pos > peopleInLine()) {
			return false;
		}
		try {
			list.insertAt(pos, p);
			return true;
		} catch (InvalidPositionException o){
			System.out.println("Invalid Position");
			return false;
		}
		 // so it compiles
	}

	/*
	 * Purpose: gets the number of people in line
	 * Parameters: none
	 * Returns: int - number of people in line
	 */
	public int peopleInLine() {
		return list.size(); // so it compiles
	}
}
public class Person {
    private String name;
	private boolean bribe;

    public Person(String name, boolean bribe) {
        this.name = name;
		this.bribe = bribe;
    }

    public String getName() {
        return name;
    }

	public boolean getBribe() {
		return bribe;
	}
}

public class Test {
    public static void main(String[] args) {
        // Creating a DisneylandLine
        DisneylandLine line = new DisneylandLine();

        // Adding people to the line
        line.enterLine(new Person("Alice", false));
        line.enterLine(new Person("Bob", false));
        line.enterLine(new Person("Charlie", false));
        line.enterLine(new Person("David", false));
        line.enterLine(new Person("Eve", false));

        // Handling one person
            Person firstPerson = line.handleOne();
            System.out.println(firstPerson.getName() + " got their ticket and entered Disneyland.");


        // Handling a group of people
        int numPeople;

        numPeople = line.handleMultiple(2);
        System.out.println(numPeople + " people got their tickets and entered Disneyland together.");


        // Adding someone with "premium" entry (bribe)
        Person briber = new Person("Frank", true);
        if (briber.getBribe()) {
			line.premiumEntry(briber, 2);
            System.out.println(briber.getName() + "bribed his way into position 2 in the line.");
        } else {
            System.out.println("Failed to add" + briber.getName() + "to the line.");
        }

        // Checking the number of people in line
        int peopleInLine = line.peopleInLine();
        System.out.println("Number of people still in line: " + peopleInLine);
    }
}
